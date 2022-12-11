Register Replication Task: ABD Algorithm
===================================

Specification:

1. The register has only **write(v)** and **read()** operation
2. Asynchronous model
3. Static configuration of 3 replicas
4. Replicas can restart and crash
5. The client knows about all replicas and their endpoints. Knows how many of them, knows their addresses
   and will communicate with them directly