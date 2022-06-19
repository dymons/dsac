Simple context-free grammar for dot language
--------------------------------------------

```
graph             : (graph | digraph) IDENTIFIER LPARAM statement_list RPARAM
statement_list    : statement SEMICOLCON statement_list | empty
statment          : node_stmt ((--,->)node_stmt)*
node_stmt         : IDENTIFIER attr_list
attr_list         : LPARAM attr (( )attr)* RPARAM | empty
attr              : IDENTIFIER ASSIGN IDENTIFIER
empty             : ''
```