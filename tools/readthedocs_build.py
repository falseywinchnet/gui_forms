#!/usr/bin/env python3
"""Ask Read the Docs to build one version of the GUI.Forms documentation.

.github/workflows/docs-publish.yml runs this once CI has passed: version
`latest` after native.yml passes on a push to main, and version `stable`
after release.yml passes on a pushed release tag. Read the Docs' own push
and pull-request builds are switched off on its side, so the site shows
only documentation of commits that CI passed.

The request is a POST to the Read the Docs API v3,
/api/v3/projects/<project>/versions/<version>/builds/. The token is read
from the environment variable READTHEDOCS_TOKEN (the repository secret of
that name). It is sent only in the Authorization header and is never
printed. The step logs the response status and the id of the build it
started.

Failure: a missing token, a request that does not reach Read the Docs, a
status other than 2xx, or a response that names no build fails the step
(exit status 1) with a message naming the cause. Nothing is retried; a
failed trigger leaves the site at its previous build.
"""
from __future__ import annotations
import argparse
import json
import os
import sys
from typing import Any, Protocol
import urllib.error
import urllib.request

API: str = 'https://readthedocs.org/api/v3'
PROJECT: str = 'gui-forms'
TOKEN_VARIABLE: str = 'READTHEDOCS_TOKEN'
TIMEOUT_SECONDS: int = 60
# Read the Docs' error bodies are short JSON; a larger body is cut before it
# is logged.
LOGGED_BODY_BOUND: int = 2048


class Response(Protocol):
    status: int

    def read(self) -> bytes: ...


class Opener(Protocol):
    def __call__(self, request: urllib.request.Request, timeout: int) -> Any: ...


def build_url(project: str, version: str) -> str:
    url: str = API + '/projects/' + project + '/versions/' + version + '/builds/'
    return url


def build_request(url: str, token: str) -> urllib.request.Request:
    """A POST with an empty JSON body; the token goes only into the Authorization header."""
    request: urllib.request.Request = urllib.request.Request(
        url, data=b'{}', method='POST',
        headers={'Authorization': 'Token ' + token, 'Content-Type': 'application/json'})
    return request


def send(request: urllib.request.Request, opener: Opener) -> tuple[int, bytes]:
    """The response status and body; an HTTP error status is a response, not an exception."""
    try:
        response: Any = opener(request, timeout=TIMEOUT_SECONDS)
        with response:
            status: int = int(response.status)
            body: bytes = response.read()
    except urllib.error.HTTPError as error:
        status = int(error.code)
        body = error.read()
    return status, body


def started_build(body: bytes) -> int | None:
    """The id of the build a 2xx response names (`build.id`), or None."""
    try:
        document: Any = json.loads(body.decode('utf-8'))
    except (UnicodeDecodeError, ValueError):
        return None
    if not isinstance(document, dict):
        return None
    build: Any = document.get('build')
    if not isinstance(build, dict):
        return None
    identifier: Any = build.get('id')
    if type(identifier) is not int:
        return None
    return identifier


def trigger(version: str, environment: dict[str, str], opener: Opener) -> int:
    """Start a build of version; the process exit status (0 started, 1 failed)."""
    token: str = environment.get(TOKEN_VARIABLE, '')
    if not token:
        print('The ' + TOKEN_VARIABLE + ' secret is not set: add a Read the Docs API token as the repository '
              'secret ' + TOKEN_VARIABLE + ' (Settings, Secrets and variables, Actions). No build was requested.',
              file=sys.stderr, flush=True)
        return 1
    url: str = build_url(PROJECT, version)
    status: int
    body: bytes
    try:
        status, body = send(build_request(url, token), opener)
    except urllib.error.URLError as error:
        print('Could not reach Read the Docs (' + url + '): ' + str(error.reason), file=sys.stderr, flush=True)
        return 1
    print('Read the Docs answered ' + str(status) + ' to the build request for ' + PROJECT + ' ' + version,
          flush=True)
    if status < 200 or status >= 300:
        print(body[:LOGGED_BODY_BOUND].decode('utf-8', errors='replace'), file=sys.stderr, flush=True)
        return 1
    identifier: int | None = started_build(body)
    if identifier is None:
        print('The response names no build', file=sys.stderr, flush=True)
        return 1
    print('Started build ' + str(identifier) + ': https://readthedocs.org/projects/' + PROJECT + '/builds/'
          + str(identifier) + '/', flush=True)
    return 0


def main(argv: list[str] | None = None) -> int:
    parser: argparse.ArgumentParser = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--version', required=True, choices=('latest', 'stable'),
                        help='the Read the Docs version to build')
    args: argparse.Namespace = parser.parse_args(argv)
    environment: dict[str, str] = dict(os.environ)
    status: int = trigger(args.version, environment, urllib.request.urlopen)
    return status


if __name__ == '__main__':
    sys.exit(main())
