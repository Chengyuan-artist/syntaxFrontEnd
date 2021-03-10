<pre>

&lt;Program&gt; ::= &lt;ExtDefList&gt;


&lt;ExtDefList&gt; ::=  &lt;ExtDef&gt; &lt;ExtDefList&gt; 
                | &lt;ExtDef&gt;
                | &lt;Blank&gt;


&lt;ExtDef&gt; ::=  &lt;ExtVarDef&gt; 
            | &lt;FuncDef&gt;


&lt;ExtVarDef&gt; ::= &lt;TypeSpecifier&gt; &lt;DeclaratorList&gt; ;


&lt;TypeSpecifier&gt; ::= int 
                  | float 
                  | char 
                  | long 
                  | double


&lt;DeclaratorList&gt; ::= &lt;Declarator&gt;, &lt;DeclaratorList&gt;


&lt;Declarator&gt; ::= &lt;Identifier&gt; 
                | &lt;Identifier&gt;[&lt;Int-constant&gt;]
                | &lt;Identifier&gt;[&lt;Long-constant&gt;]


&lt;FuncDef&gt; ::= &lt;TypeSpecifier&gt; &lt;Identifier&gt; ( &lt;FormalParaList&gt; ) { &lt;CompoundStatementList&gt; }
            | &lt;TypeSpecifier&gt; &lt;Identifier&gt; ( &lt;FormalParaList&gt; );
          
          
          
&lt;FormalParaList&gt; ::= &lt;FormalPara&gt;, &lt;FormalParaList&gt; 
                   | &lt;FormalPara&gt; 
                   | &lt;Blank&gt;


&lt;FormalPara&gt; ::=  &lt;TypeSpecifier&gt; &lt;Identifier&gt;



&lt;CompoundStatementList&gt; ::= &lt;CompoundStatement&gt;  &lt;CompoundStatementList&gt;
                           |&lt;CompoundStatement&gt;



&lt;CompoundStatement&gt; ::=  &lt;LocalVarDef&gt; 
                          | &lt;Statement&gt; 


&lt;LocalVarDef&gt; ::= &lt;TypeSpecifier&gt; &lt;DeclaratorList&gt; ;


&lt;Statement&gt; ::=   &lt;Expression&gt;; 
                | return &lt;Expression&gt;; 
                | { &lt;CompoundStatementList&gt; }
                | if ( &lt;Expression&gt; ) &lt;Statement&gt; 
                | if ( &lt;Expression&gt; ) &lt;Statement&gt; else &lt;Statement&gt;
                | while ( &lt;Expression&gt; ) &lt;Statement&gt;
                | for (&lt;Expression&gt;?;&lt;Expression&gt;?;&lt;Expression&gt;?) &lt;Statement&gt;
                | break;
                | continue;
             
             
                
&lt;Expression&gt; ::=  &lt;Expression&gt; &lt;Operator&gt; &lt;Expression&gt; 
                | &lt;Constant&gt; 
                | &lt;Assignable-lvalue&gt; 
                | &lt;Identifier&gt; (&lt;ArgumentsList&gt;)
                | &lt;Assignment-expression&gt;



&lt;ArgumentsList&gt; ::=   &lt;Expression&gt;, &lt;ArgumentsList&gt; 
                    | &lt;Expression&gt; 
                    | &lt;Blank&gt;


&lt;Operator&gt; ::=  + 
              | - 
              | * 
              | / 
              | %
              | > 
              | >= 
              | < 
              | <= 
              | == 
              | != 
              | || 
              | &&


&lt;Constant&gt; ::=  &lt;Int-constant&gt; 
             |  &lt;Long-constant&gt; 
             |  &lt;Float-constant&gt; 
             |  &lt;Double-constant&gt; 
             |  &lt;Char-constant&gt; 


&lt;Assignment-expression&gt; ::=  &lt;Assignable-lvalue&gt; = &lt;Expression&gt;
                            
                            
&lt;Assignable-lvalue&gt; ::=  &lt;Identifier&gt;
                       | &lt;Identifier&gt;[&lt;Expression&gt;]
               

</pre>



