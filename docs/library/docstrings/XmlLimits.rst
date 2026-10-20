The bounds that `read_xml` and `write_xml` enforce.

Every bound has its own error, so a refused document says which bound it
crossed. The defaults suit configuration files such as saved layouts: 4 MiB
of input, depth 64, 65536 elements, 64 attributes per element, names of 256
bytes and values of 64 KiB. Lengths are counted in bytes of UTF-8 after
entities are decoded.

See Also
--------
XmlElement : The tree a read produces and a write consumes.

Notes
-----
``maximum_input_bytes`` bounds the document, including its byte order mark
(`XmlError::input_too_large`); the writer refuses to produce a document the
reader would refuse. ``maximum_depth`` counts the root as depth 1
(`XmlError::depth_exceeded`); it must be between 1 and
``xml_depth_ceiling`` (1024), since a tree is destroyed recursively,
otherwise `XmlError::invalid_limits`. ``maximum_elements`` bounds the
elements in the whole document (`XmlError::element_limit_exceeded`),
``maximum_attributes_per_element`` the attributes of one element
(`XmlError::attribute_limit_exceeded`), ``maximum_name_bytes`` one element
or attribute name (`XmlError::name_too_long`) and ``maximum_value_bytes``
one attribute value, one element's text or one comment
(`XmlError::value_too_long`).

Together the limits bound what a read can hold: the tree is a small
multiple of the input's size. The test suite measures the heap requested by
reads of mutated and of deliberately dense documents and requires at most
128 bytes per input byte plus 64 KiB.

Examples
--------
Tighten a bound and see it enforced:

.. code-block:: cpp

   gf::XmlLimits limits;
   limits.maximum_depth = 2;
   gf::XmlReadResult shallow = gf::read_xml("<a><b/></a>", limits);        // accepted
   gf::XmlReadResult deep = gf::read_xml("<a><b><c/></b></a>", limits);   // refused
   bool refused = deep.error == gf::XmlError::depth_exceeded;              // true
   static_cast<void>(shallow);
   static_cast<void>(refused);
