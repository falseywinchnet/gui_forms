"""Read the Docs is asked to build only with a token, and the token never reaches the log."""
from __future__ import annotations
import contextlib
import io
import json
from typing import Any
import unittest
import urllib.error
import urllib.request

import readthedocs_build

TOKEN: str = 'secret-token-value'


class FakeResponse:
    def __init__(self, status: int, body: bytes) -> None:
        self.status: int = status
        self.body: bytes = body

    def read(self) -> bytes:
        return self.body

    def __enter__(self) -> FakeResponse:
        return self

    def __exit__(self, *details: Any) -> None:
        return None


class FakeOpener:
    """Answers each request with status and body, or raises error; records the requests."""

    def __init__(self, status: int = 202, body: bytes = b'', error: Exception | None = None) -> None:
        self.status: int = status
        self.body: bytes = body
        self.error: Exception | None = error
        self.requests: list[urllib.request.Request] = []

    def __call__(self, request: urllib.request.Request, timeout: int) -> FakeResponse:
        self.requests.append(request)
        if self.error is not None:
            raise self.error
        return FakeResponse(self.status, self.body)


def started(identifier: int) -> bytes:
    body: bytes = json.dumps({'build': {'id': identifier, 'state': {'code': 'triggered'}},
                              'project': {'slug': 'gui-forms'}, 'version': {'slug': 'latest'}}).encode('utf-8')
    return body


class TriggerTests(unittest.TestCase):
    def run_trigger(self, version: str, environment: dict[str, str], opener: FakeOpener) -> tuple[int, str]:
        output: io.StringIO = io.StringIO()
        with contextlib.redirect_stdout(output), contextlib.redirect_stderr(output):
            status: int = readthedocs_build.trigger(version, environment, opener)
        return status, output.getvalue()

    def test_a_started_build_is_logged_by_status_and_id_without_the_token(self) -> None:
        opener: FakeOpener = FakeOpener(202, started(4242))
        status: int
        log: str
        status, log = self.run_trigger('latest', {'READTHEDOCS_TOKEN': TOKEN}, opener)
        self.assertEqual(status, 0)
        self.assertIn('answered 202', log)
        self.assertIn('Started build 4242', log)
        self.assertNotIn(TOKEN, log)
        self.assertEqual(len(opener.requests), 1)
        request: urllib.request.Request = opener.requests[0]
        self.assertEqual(request.get_method(), 'POST')
        self.assertEqual(request.full_url, 'https://readthedocs.org/api/v3/projects/gui-forms/versions/latest/builds/')
        self.assertEqual(request.get_header('Authorization'), 'Token ' + TOKEN)

    def test_the_stable_version_has_its_own_endpoint(self) -> None:
        opener: FakeOpener = FakeOpener(202, started(7))
        self.assertEqual(self.run_trigger('stable', {'READTHEDOCS_TOKEN': TOKEN}, opener)[0], 0)
        self.assertTrue(opener.requests[0].full_url.endswith('/versions/stable/builds/'))

    def test_a_missing_or_empty_token_fails_clearly_without_a_request(self) -> None:
        environment: dict[str, str]
        for environment in ({}, {'READTHEDOCS_TOKEN': ''}):
            opener: FakeOpener = FakeOpener(202, started(1))
            status: int
            log: str
            status, log = self.run_trigger('latest', environment, opener)
            self.assertEqual(status, 1)
            self.assertIn('READTHEDOCS_TOKEN secret is not set', log)
            self.assertEqual(opener.requests, [])

    def test_a_refused_request_fails_with_its_status_and_no_token(self) -> None:
        refusal: urllib.error.HTTPError = urllib.error.HTTPError(
            readthedocs_build.build_url('gui-forms', 'latest'), 401, 'Unauthorized', None,  # type: ignore[arg-type]
            io.BytesIO(b'{"detail": "Invalid token."}'))
        status: int
        log: str
        status, log = self.run_trigger('latest', {'READTHEDOCS_TOKEN': TOKEN}, FakeOpener(error=refusal))
        self.assertEqual(status, 1)
        self.assertIn('answered 401', log)
        self.assertIn('Invalid token.', log)
        self.assertNotIn(TOKEN, log)

    def test_an_unreachable_service_fails(self) -> None:
        status: int
        log: str
        status, log = self.run_trigger('latest', {'READTHEDOCS_TOKEN': TOKEN},
                                       FakeOpener(error=urllib.error.URLError('no route')))
        self.assertEqual(status, 1)
        self.assertIn('Could not reach Read the Docs', log)
        self.assertNotIn(TOKEN, log)

    def test_a_success_that_names_no_build_fails(self) -> None:
        body: bytes
        for body in (b'', b'not json', b'[]', b'{"build": {}}', b'{"build": {"id": "4"}}'):
            self.assertEqual(self.run_trigger('latest', {'READTHEDOCS_TOKEN': TOKEN}, FakeOpener(202, body))[0], 1)


if __name__ == '__main__':
    unittest.main()
