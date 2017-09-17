# Requirements

Register with the shell to handle URIs described in [rfc3986][1]. Only part 
of the RFC needs to be implemented, the following parts are recognized:
proto:[//]authority[/path][?query][#fragment].

| rfc3986 | function                             |
|---------|--------------------------------------|
| proto   | used by the shell to map to this exe |
| authority | used to define the action |
| path | used to define the sub action |
| query | holds all parameters for the executable |
| fragment | placeholder |

- open documents
- run executables
- replace text in config files
- sanitize and check paths
- logging

# Open Documents

[1]: https://tools.ietf.org/html/rfc3986