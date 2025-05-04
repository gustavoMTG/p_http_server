curl -H "Connection: keep-alive" -X GET http://localhost:8080/makefile
curl -H "Connection: keep-alive" -X GET http://localhost:8080/README.md
curl -H "close" -X GET http://localhost:8080/favicon.ico
