# include <iostream>
# include <stdio.h>
# include <string>
# include <vector>
# include <stdlib.h>
# include <cstdlib>
# include <iomanip>
# include <exception>
# include <cmath>

using namespace std;

int gLine = 1, gColumn = 0;

enum Type{
  END_OF_FILE, NONE, IDENT, NUM, OPERATOR, INT, FLOAT, CHAR, BOOL, 
  STRING, VOID, IF, ELSE, WHILE, DO, RETURN, GE, LE, EQ, ADD, SUB, 
  THAN, LESS, NEQ, AND, OR, PE, ME, TE, DE, RE, PP, MM, RS, LS, LP, 
  RP, LMP, RMP, LBP, RBP, XOR, SEM, COM, QUE, COL, MULT, DIV, ASSIGN, 
  PER, LAND, LOR, EXC, CONST
};

struct Token {
  Type type ;
  string str;
  Type value_type;
  double value;
};

struct Array {
  Type type ; // void | int | char | float | string | bool
  string name;
  int length;
  vector<Token> items;
};

struct Parameter{
  Type type ;
  int num;
  string name;
  bool isArray; // default false
  bool passAddr; // default false
  Token token;
  Array array;
};

struct Function {
  Type type ; // void | int | char | float | string | bool
  string name;
  vector<Parameter> parameter;
  vector<Token> statements; 
};

struct Variable {
  Type type ; // int | char | float | string | bool
  string name;
  bool isArray; // default false
  Token token;
  Array array;
};

vector<Variable> gGlobalVariable;
vector<Variable> gLocalVariable;
vector<Function> gFunctionTable;
vector<Array> gArrayTable; 
vector<Token> gTempStatements;
int gERRORCODE = 0;
Token gUnDealToken;
Token gLogToken;
bool gPROGRAMEND = false;
bool gDEBUGMODE = false;
bool gLogStatement = false;
bool gPeeking = false;
bool gInFunction = false;
int gCompoundTime = 0;
string gFunctionName = "";
string gParameter = "";

void ErrorHandler( string str, int errorType ) ;
void PrintValue( Token token ) ;
void User_input( bool & correct ) ;
void Definition( bool & correct, Token & token ) ;
void Type_specifier( bool & correct, Token & token ) ;
void Function_definition_or_declarators( bool & correct, string type, string name ) ;
void Function_definition_without_ID( bool & correct, string functionType, string functionName ) ;
void Rest_of_declarators( bool & correct, Token & token, string type,
                          string name, bool global, bool print ) ;
void Formal_parameter_list( bool & correct, Function & func ) ;
void Compound_Statement( bool & correct, Function & func, Token & token ) ;
void Declaration( bool & correct, bool print ) ;
void Statement(  bool & correct, Token & token, bool print  ) ;
void Expression( bool & correct, Token & token ) ;
void Basic_expression( bool & correct, Token & token ) ;
void Rest_of_ID_started_basic_exp( bool & correct, Token & token ) ;
void Rest_of_PPMM_ID_started_basic_exp( bool & correct, Token & token ) ;
void Signed_unary_exp( bool & correct, Token & token ) ;
void Romce_and_romloe( bool & correct, Token & token ) ;
void Actual_parameter_list( bool & correct, Token & token ) ;
void Assignment_operator( bool & correct, Token & token ) ;
void Romce_and_romloe( bool & correct, Token & token ) ;
void Rest_of_maybe_logical_OR_exp( bool & correct, Token & token ) ;
void Rest_of_maybe_logical_AND_exp( bool & correct, Token & token ) ;
void Maybe_logical_AND_exp( bool & correct, Token & token ) ;
void Maybe_bit_OR_exp( bool & correct, Token & token ) ;
void Rest_of_maybe_bit_OR_exp( bool & correct, Token & token ) ;
void Maybe_bit_ex_OR_exp( bool & correct, Token & token ) ;
void Rest_of_maybe_bit_ex_OR_exp( bool & correct, Token & token ) ;
void Maybe_bit_AND_exp( bool & correct, Token & token ) ;
void Rest_of_maybe_bit_AND_exp( bool & correct, Token & token ) ;
void Maybe_equality_exp( bool & correct, Token & token ) ;
void Rest_of_maybe_equality_exp( bool & correct, Token & token ) ;
void Maybe_relational_exp( bool & correct, Token & token ) ;
void Rest_of_maybe_relational_exp( bool & correct, Token & token ) ;
void Maybe_shift_exp( bool & correct, Token & token ) ;
void Rest_of_maybe_shift_exp( bool & correct, Token & token ) ;
void Maybe_additive_exp( bool & correct, Token & token ) ;
void Rest_of_maybe_additive_exp( bool & correct, Token & token ) ;
void Maybe_mult_exp( bool & correct, Token & token ) ;
void Rest_of_maybe_mult_exp( bool & correct, Token & token ) ;
void Unary_exp( bool & correct, Token & token ) ;
void Signed_unary_exp( bool & correct, Token & token ) ;
void Unsigned_unary_exp( bool & correct, Token & token ) ;
void Sign( bool & correct, Token & token ) ;


Type String_to_enum( string str ) {
  if ( str == "int" ) {
    return INT;
  } // if
  else if ( str == "char" ) {
    return CHAR;
  } // else if
  else if ( str == "float" ) {
    return FLOAT;
  } // else if
  else if ( str == "string" ) {
    return STRING;
  } // else if
  else if ( str == "bool" ) {
    return BOOL;
  } // else if
  else if ( str == "void" ) {
    return VOID;
  } // else if
  
  return NONE;
} // String_to_enum()

string Enum_to_string( Type type ) {
  if ( type == INT ) {
    return "int";
  } // if
  else if ( type == CHAR ) {
    return "char";
  } // else if
  else if ( type == FLOAT ) {
    return "float";
  } // else if
  else if ( type == STRING ) {
    return "string";
  } // else if
  else if ( type == BOOL ) {
    return "bool";
  } // else if
  else if ( type == VOID ) {
    return "void";
  } // else if
  
  return "none";
} // Enum_to_string()

bool IsWhiteSpace( char ch ) {
  if ( ch == ' ' || ch == '\t' || ch == '\n' ) {
    return true;
  } // if
  
  return false;
} // IsWhiteSpace()

void Initialize( Token & token ) {
  token.type = NONE;
  token.str = "";
  token.value_type = NONE;
  token.value = 0.0;
} // Initialize()

void InitializeArray( Array & array ) {
  array.items.clear();
  array.length = 0;
  array.name = "";
  array.type = NONE;
} // InitializeArray()

void InitializeParameter( Parameter & par ) {
  InitializeArray( par.array );
  par.isArray = false;
  par.name = "";
  par.num = 0;
  par.passAddr = false;
  Initialize( par.token );
  par.type = NONE;
} // InitializeParameter()

void InitializeVariable( Variable & var ) {
  InitializeArray( var.array );
  var.isArray = false;
  var.name = "";
  Initialize( var.token );
  var.type = NONE;
} // InitializeVariable()

void BubbleSortVar( vector< Variable > & a ) {
  Variable temp;
  for ( int i = 0; i < a.size() ; i++ ) {
    bool swapped = false;
    for ( int j = 0; j < a.size() - i - 1 ; j++ ) {
      if ( a.at( j ).name > a.at( j + 1 ).name ) {
        temp = a.at( j );
        a.at( j ) = a.at( j + 1 );
        a.at( j + 1 ) = temp;
        swapped = true;
      } // if   
      
    } // for
    
  } // for
  
} // BubbleSortVar()

void BubbleSort( vector < Function > & a ) {
  Function temp;
  for ( int i = 0; i < a.size() ; i++ ) {
    bool swapped = false;
    for ( int j = 0; j < a.size() - i - 1 ; j++ ) {
      if ( a.at( j ).name > a.at( j + 1 ).name ) {
        temp = a.at( j );
        a.at( j ) = a.at( j + 1 );
        a.at( j + 1 ) = temp;
        swapped = true;
      } // if   
      
    } // for
    
  } // for
  
} // BubbleSort()

void ListAllVariables() {
  string smallest = "";
  BubbleSortVar( gGlobalVariable );
  for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
    cout << gGlobalVariable.at( i ).name << "\n";
  } // for
  
} // ListAllVariables()

void ListAllFunctions() {
  string smallest = "";
  BubbleSort( gFunctionTable );
  for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
    cout << gFunctionTable.at( i ).name << "()\n";
  } // for
  
} // ListAllFunctions()

void ListVariable( string name ) {
  for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
    if ( name == gGlobalVariable.at( i ).name ) {
      cout << Enum_to_string( gGlobalVariable.at( i ).type ) 
      << " " << gGlobalVariable.at( i ).name ;
      if ( gGlobalVariable.at( i ).isArray ) {
        cout << "[ " << gGlobalVariable.at( i ).array.length << " ]";
      } // if
      
      cout << " ;\n";
    } // if
    
  } // for
  
} // ListVariable()

void ListFunction( string name ) {
  int whiteSpaceTime = 0;
  bool inIf = false, inElse = false;
  for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
    if ( name == gFunctionTable.at( i ).name ) {
      cout << Enum_to_string( gFunctionTable.at( i ).type ) << " ";
      cout << gFunctionTable.at( i ).name << "(";
      if ( gFunctionTable.at( i ).parameter.size() > 0 ) {
        cout << " ";
        for ( int j = 0; j < gFunctionTable.at( i ).parameter.size() ; j++ ) {
          // cout << gFunctionTable.at(i).parameter.at(j).type << " ";
          if ( gFunctionTable.at( i ).parameter.at( j ).name == "void" ) {
            cout << "void";
          } // if
          else {
            cout << Enum_to_string( gFunctionTable.at( i ).parameter.at( j ).type ) << " ";
            if ( gFunctionTable.at( i ).parameter.at( j ).passAddr ) {
              cout << "& ";
            } // if
            
            cout << gFunctionTable.at( i ).parameter.at( j ).name;
          } // else
          
          if ( gFunctionTable.at( i ).parameter.at( j ).isArray ) {
            cout << "[ ";
            cout << gFunctionTable.at( i ).parameter.at( j ).array.length;
            cout << " ]";
          } // if
  
          if ( j < gFunctionTable.at( i ).parameter.size() - 1 ) {
            cout << ", ";
          } // if
          else {
            cout << " ";
          } // else
  
        } // for
      } // if
      else {
        cout << " ";
      } // else      

      cout << ") ";
      for ( int j = 0; j < gFunctionTable.at( i ).statements.size() ; j++ ) {
        string toPrint = gFunctionTable.at( i ).statements.at( j ).str;
        if ( toPrint == "{" ) {
          whiteSpaceTime++;
          cout << toPrint << "\n";
          if ( j != gFunctionTable.at( i ).statements.size() - 2 ) {
            // 當進入compound statement/ if...else/while/do...while時要多印空白 
            for ( int t = whiteSpaceTime; t > 0 ; t-- ) {
              cout << "  ";
            } // for
            
          } // if

        } // if
        else if ( toPrint == ")" ) {
          if ( inIf ) {
            if ( gFunctionTable.at( i ).statements.at( j + 1 ).str != "{" ) {
              whiteSpaceTime++;
              if ( gFunctionTable.at( i ).statements.at( j + 1 ).str != ")" ) {
                cout << toPrint << "\n";
                for ( int t = whiteSpaceTime; t > 0 ; t-- ) {
                  cout << "  ";
                } // for
              } // if
              else {
                cout << toPrint << " " ;
              } // else              
              
              if ( gFunctionTable.at( i ).statements.at( j + 1 ).str != "if" &&
                   gFunctionTable.at( i ).statements.at( j + 1 ).str != "while" &&
                   gFunctionTable.at( i ).statements.at( j + 1 ).str != "do" ) {
                whiteSpaceTime--;
                inIf = false;
              } // if
              
            } // if
            else {
              cout << toPrint << " ";
            } // else
            
          } // if
          else {
            cout << toPrint << " ";
          } // else
                      
        } // else if
        else if ( toPrint == "else" ) {
          if ( gFunctionTable.at( i ).statements.at( j + 1 ).str != "{" ) {
            whiteSpaceTime++;
            cout << toPrint << "\n";
            for ( int t = whiteSpaceTime; t > 0 ; t-- ) {
              cout << "  ";
            } // for
            
            whiteSpaceTime--;
            inIf = false;
          } // if
          else {
            cout << toPrint << " ";
          } // else
                                  
        } // else if
        else if ( toPrint == "}" ) {
          whiteSpaceTime--;
          cout << toPrint << "\n";
          if ( j != gFunctionTable.at( i ).statements.size() - 1 ) {
            if ( gFunctionTable.at( i ).statements.at( j + 1 ).str != "}" ) {
              if ( j != gFunctionTable.at( i ).statements.size() -2 ) {
                for ( int t = whiteSpaceTime; t > 0 ; t-- ) {
                  cout << "  ";
                } // for
                
              } // if
              
            } // if
            else {
              if ( j != gFunctionTable.at( i ).statements.size() -2 ) {
                for ( int t = whiteSpaceTime - 1 ; t > 0 ; t-- ) {
                  cout << "  ";
                } // for
                
              } // if
              
            } // else
          } // if
            
          
            
        } // else if
        else if ( toPrint == ";" ) {
          cout << toPrint << "\n";
          if ( gFunctionTable.at( i ).statements.at( j + 1 ).str != "}" ) {
            // 當進入compound statement/ if...else/while/do...while時要多印空白 
            for ( int t = whiteSpaceTime; t > 0 ; t-- ) {
              cout << "  ";
            } // for
            
          } // if
          else {
            for ( int t = whiteSpaceTime - 1 ; t > 0 ; t-- ) {
              cout << "  ";
            } // for
          } // else

        } // else if
        else {
          if ( gFunctionTable.at( i ).statements.at( j ).type == IDENT &&
               ( gFunctionTable.at( i ).statements.at( j + 1 ).str == "[" ||
                 gFunctionTable.at( i ).statements.at( j + 1 ).str == "(" ||
                 gFunctionTable.at( i ).statements.at( j + 1 ).str == "++" ||
                 gFunctionTable.at( i ).statements.at( j + 1 ).str == "--" ) ) {
            if ( gFunctionTable.at( i ).statements.at( j ).str == "do" ||
                 gFunctionTable.at( i ).statements.at( j ).str == "while" ||
                 gFunctionTable.at( i ).statements.at( j ).str == "if" ||
                 gFunctionTable.at( i ).statements.at( j ).str == "return" ||
                 gFunctionTable.at( i ).statements.at( j ).str == "else" ) {
              if ( gFunctionTable.at( i ).statements.at( j ).str == "if" ||
                   gFunctionTable.at( i ).statements.at( j ).str == "while" ) {
                inIf = true;
              } // if
              
              cout << toPrint << " ";
            } // if
            else {
              cout << toPrint ;
            } // else
            
          } // if
          else {
            
            if ( gFunctionTable.at( i ).statements.at( j ).type == IDENT &&
                 gFunctionTable.at( i ).statements.at( j + 1 ).str == "," ) {
              cout << toPrint ;
            } // if
            else if ( gFunctionTable.at( i ).statements.at( j ).str == "]" &&
                      gFunctionTable.at( i ).statements.at( j + 1 ).str == "," ) {
              cout << toPrint ;
            } // else if
            else {
              cout << toPrint << " ";
            } // else
            
          } // else
          
        } // else

      } // for

    } // if

  } // for

} // ListFunction()

void FunctionHandler( string funcName, string par, bool print ) {
  string realPar = "";
  if ( print )
    cout << "> ";

  if ( funcName == "ListAllVariables" ) {
    if ( gDEBUGMODE )
      cout << "ListAllVariables\n";
      
    ListAllVariables();
  } // if
  else if ( funcName == "ListAllFunctions" ) {
    if ( gDEBUGMODE )
      cout << "ListAllFunctions\n";
      
    ListAllFunctions();
  } // else if
  else if ( funcName == "ListVariable" ) {
    if ( gDEBUGMODE )
      cout << "ListVariable\n";
    
    for ( int i = 0; i < par.size() ; i++ ) {
      if ( i != 0 && i != par.size() - 1 ) {
        realPar += par.at( i );
      } // if
      
    } // for
    
    ListVariable( realPar );
  } // else if
  else if ( funcName == "ListFunction" ) {
    if ( gDEBUGMODE )
      cout << "ListFunction\n";
      
    for ( int i = 0; i < par.size() ; i++ ) {
      if ( i != 0 && i != par.size()-1 ) {
        realPar += par.at( i );
      } // if
    } // for
    
    ListFunction( realPar );
  } // else if 
  else if ( funcName == "Done" ) {
    cout << "Our-C exited ...";
    exit( 0 );
  } // else if

  gFunctionName = "";
  gParameter = ""; 
} // FunctionHandler()

void PrintValue( Token token ) {
  if ( token.value_type == BOOL && token.value == 1 ) {
    cout << "> true\n";
  } // if
  else if ( token.value_type == BOOL && token.value == 0 ) {
    cout << "> false\n";
  } // else if
  else if ( token.value_type == FLOAT ) {
    // cout << value << "\n";
    printf( "> %.3f\n", token.value );
  } // else if
  else if ( token.value_type == INT ) {
    cout << "> " << token.value << "\n";
  } // else if

} // PrintValue()

char Get_the_next_char() {
  // to read a character 
  // and keep track of the line number and gcolumn number of this character
  // return this character
  char ch = '\0';
  ch = cin.get();
  // cout << "AAA" << ch << "AAA";
  if ( ch == EOF ) {
    return ch;
  } // if
  else if ( ch == '\n' ) {
    ++gLine;
    gColumn = 0;
    return ch;
  } // else if
  else {
    ++gColumn;
    return ch;
  } // else 

} // Get_the_next_char()

char Get_the_next_non_white_space_char() {
  // to read a non-white-space character 
  // and skip comment
  // return this character
  char ch = '\0', comment = '\0';
  ch = Get_the_next_char();
  while ( IsWhiteSpace( ch ) ) {
    ch = Get_the_next_char();
  } // while

  while ( ch == '/' ) {
    comment = cin.peek();
    // if comment is '/', then read until '\n'.
    if ( comment == '/' ) {
      comment = '\0';
      ch = Get_the_next_char();
      while ( ch != '\n' ) { // read until '\n'.
        ch = Get_the_next_char();
      } // while
      
      while ( IsWhiteSpace( ch ) ) { // read until not white space
        ch = Get_the_next_char();
      } // while
      
    } // if
    else { // comment is not '/'(it's means it's only a '/' character 這只是一個除號)
      return ch;
    } // else
    
    // if the next not white space(ch) is '/', then will see if it is comment or not(while loop again).
  } // while
    
  return ch;
} // Get_the_next_non_white_space_char()

Token Get_all_ident( char firstCharacter ) {
  // F3 
  // An IDENT starts with a letter and followed by digits or underglines
  Token token;
  Variable var;
  char ch = '\0';
  string tokenStr = "";
  Initialize( token );
  
  tokenStr += firstCharacter;
  
  ch = cin.peek();
  while ( ( ch >= 'A' && ch <= 'Z' ) || ( ch >= 'a' && ch <= 'z' ) 
          || ( ch >= '0' && ch <= '9' ) || ch == '_' ) {
    ch = Get_the_next_char();
    tokenStr += ch;
    ch = cin.peek();
  } // while
  
  token.type = IDENT;
  token.str = tokenStr;
  if ( token.str == "true" ) {
    token.type = IDENT;
    token.str = "true";
    token.value = 1;
    token.value_type = BOOL;
    var.isArray = false;
    var.name = "true";
    var.type = IDENT;
    var.token = token;
    gGlobalVariable.push_back( var ); 
  } // if
  else if ( token.str == "false" ) {
    token.type = IDENT;
    token.str = "false";
    token.value = 0;
    token.value_type = BOOL;
    var.isArray = false;
    var.name = "false";
    var.type = IDENT;
    var.token = token;
    gGlobalVariable.push_back( var ); 
  } // else if
  return token;

} // Get_all_ident()

Token Get_all_constant( char firstCharacter ) {
  // F5
  // A num can be a integer or a float
  // e.g. 35, 7, 43.8, 43., .35, 1.0, 07

  Token token;
  char ch = '\0';
  bool isFloat = false;
  string tokenStr = "";
  Initialize( token );
  token.type = CONST;
  token.value_type = INT;
  
  tokenStr += firstCharacter;
  if ( tokenStr == "." ) { // To see if there is only a '.'.
    ch = cin.peek();
    if ( ch >= '0' && ch <= '9' ) {
      isFloat = true;
      token.value_type = FLOAT;
    } // if
    else {
      token.str = tokenStr;
      token.type = OPERATOR;
      return token;
    } // else

  } // if
  
  ch = cin.peek();
  while ( ( ch >= '0' && ch <= '9' ) || ch == '.' ) {
    if ( ch == '.' ) { 
      if ( isFloat == true ) { // another '.' occur, like: 3.4.5 or 3.. or .3.
        token.str = tokenStr;
        token.value = atof( tokenStr.c_str() );
        return token;
      } // if
      else {
        isFloat = true;
        token.value_type = FLOAT;
      } // else
    } // if

    ch = Get_the_next_char();
    tokenStr += ch;
    ch = cin.peek();
  } // while
  
  token.str = tokenStr;
  token.value = atof( tokenStr.c_str() );
  return token;
} // Get_all_constant()

Token Get_all_operator( char firstCharacter ) {
  // F4
  // A SIGN can be '+' or '-' or '*' or '/'.....
  Token token;
  char ch = '\0';
  string tokenStr = "";
  Initialize( token );
  token.type = OPERATOR;
  
  tokenStr += firstCharacter;
  
  if ( tokenStr == "(" ) {
    token.type = LP;
    token.str = tokenStr;
    return token;
  } // if
  else if ( tokenStr == ")" ) {
    token.type = RP;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "[" ) {
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "]" ) {
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "{" ) {
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "}" ) {
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "+" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = PE;
      token.str = tokenStr;
      return token;
    } // if
    else if ( ch == '+' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = PP;
      token.str = tokenStr;
      return token;
    } // else if
    else {
      token.type = ADD;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "-" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = ME;
      token.str = tokenStr;
      return token;
    } // if
    else if ( ch == '-' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = MM;
      token.str = tokenStr;
      return token;
    } // else if
    else {
      token.type = SUB;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "*" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = TE;
      token.str = tokenStr;
      return token;
    } // if
    else {
      token.type = MULT;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "/" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = DE;
      token.str = tokenStr;
      return token;
    } // if
    else {
      token.type = DIV;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "%" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = RE;
      token.str = tokenStr;
      return token;
    } // if
    else {
      token.type = PER;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "^" ) {
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == ">" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = GE;
      token.str = tokenStr;
      return token;
    } // if
    else if ( ch == '>' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = RS;
      token.str = tokenStr;
      return token;
    } // else if
    else {
      token.type = THAN;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "<" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = LE;
      token.str = tokenStr;
      return token;
    } // if
    else if ( ch == '<' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = LS;
      token.str = tokenStr;
      return token;
    } // else if
    else {
      token.type = LESS;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "=" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = GE;
      token.str = tokenStr;
      return token;
    } // if
    else {
      token.type = ASSIGN;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "!" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = NEQ;
      token.str = tokenStr;
      return token;
    } // if
    else {
      token.type = EXC;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "&" ) {
    ch = cin.peek();
    if ( ch == '&' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = AND;
      token.str = tokenStr;
      return token;
    } // if
    else {
      token.type = LAND;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == "|" ) {
    ch = cin.peek();
    if ( ch == '|' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = OR;
      token.str = tokenStr;
      return token;
    } // if
    else {
      token.type = LOR;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  else if ( tokenStr == ";" ) {
    token.type = SEM;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "," ) {
    token.type = SEM;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "?" ) {
    token.type = SEM;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == ":" ) {
    token.type = COL;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "\"" ) {
    ch = cin.peek();
    if ( ch != '\n' ) {
      while ( ch != '\"' ) {
        ch = Get_the_next_char();
        tokenStr += ch;
        ch = cin.peek();
        if ( ch == '\n' ) {
          ErrorHandler( tokenStr, 1 );
        } // if
      } // while
    } // if
    
    ch = Get_the_next_char();
    tokenStr += ch;
    token.type = CONST;
    token.str = tokenStr;
    token.value_type = STRING;
    return token;
  } // else if
  else if ( tokenStr == "\'" ) {
    ch = cin.peek();
    if ( ch != '\'' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      ch = cin.peek();
      if ( ch == '\'' ) {
        ch = Get_the_next_char();
        tokenStr += ch;
      } // if
      else {
        ErrorHandler( tokenStr, 1 );
      } // else
    } // if
    else {
      ch = Get_the_next_char();
      tokenStr += ch;
    } // else
    
    token.type = CONST;
    token.value_type = CHAR;
    token.str = tokenStr;
    return token;
  } // else if
  else {
    ErrorHandler( tokenStr, 1 );
  } // else

  return token;
} // Get_all_operator()

Token GetToken() {
  char ch = '\0';
  Token token;
  Initialize( token );
  if ( gUnDealToken.type != NONE ) {
    // 有尚未處理的token, 先拿尚未處理的token來用
    token = gUnDealToken;
    Initialize( gUnDealToken );
  } // if
  else {
    // 沒有尚未處理的token, 從input取資料 
    ch = Get_the_next_non_white_space_char();
    if ( ch == EOF ) {
      token.type = END_OF_FILE;
    } // if
    else if ( ( ch >= 'A' && ch <= 'Z' ) || ( ch >= 'a' && ch <= 'z' ) || ch == '_' ) {
      token = Get_all_ident( ch );
    } // else if
    else if ( ( ch >= '0' && ch <= '9' ) || ch == '.' ) {
      token = Get_all_constant( ch );
    } // else if
    else {
      // the ERROR: Unrecognized token will occur in Get_all_operator() function.
      token = Get_all_operator( ch );
    } // else
    
  } // else
  
  if ( gLogStatement && !gPeeking ) {
    gTempStatements.push_back( token );
  } // if
  
  return token;
} // GetToken()

void PeekToken() {
  gPeeking = true;
  gUnDealToken = GetToken();
  gPeeking = false;
} // PeekToken()

void ErrorHandler( string str, int errorType ) {
  char ch = '\0';
  if ( errorType == 0 ) {
    cout << "> Line " << gLine << " : Error\n" ;
    // do nothing
  } // if
  else if ( errorType == 1 ) {
    cout << "> Line " << gLine << " : unrecognized token with first char : '" + str +  "'\n";
  } // else if
  else if ( errorType == 2 ) {
    cout << "> Line " << gLine << " : unexpected token : '" + str +  "'\n";
  } // else if
  else { 
    cout << "> Line " << gLine << " : undefined identifier : '" + str +  "'\n";
  } // else
  
  gLine = 0;
  while ( ch != '\n' ) {
    ch = Get_the_next_char();
  } // while
  
  gInFunction = false;
  gCompoundTime = 0;
  Initialize( gUnDealToken );
  throw errorType;
} // ErrorHandler()

void User_input( bool & correct ) {
  // (Definition | Statement) { Definition | Statement }
  if ( gDEBUGMODE )
    cout << "User_input\n";
  
  bool definitionCorrect = false, statementCorrect = false;
  Token token;
  Initialize( token );
  Definition( definitionCorrect, token );
  if ( !definitionCorrect ) {
    Statement( statementCorrect, token, true );
    if ( !statementCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
  } // if
  
  while ( true ) {
    definitionCorrect = false;
    statementCorrect = false;
    Definition( definitionCorrect, token );
    if ( !definitionCorrect ) {
      Statement( statementCorrect, token, true );
      if ( !statementCorrect ) {
        return;
      } // if
    
    } // if
    
    
  } // while
  
  correct = true;
} // User_input()

void Definition( bool & correct, Token & token ) {
  // VOID Identifier function_definition_without_ID
  // | type_specifier Identifier function_definition_or_declarators
  if ( gDEBUGMODE )
    cout << "Definition\n";
   
  correct = false;
  Token temp, token1;
  string type = "", name = "";
  bool type_specifierCorrect = false;
  bool fdwiCorrect = false;
  bool fdodCorrect = false;
  // gLine = 1;
  Initialize( temp );
  Initialize( token );
  PeekToken();
  if ( gUnDealToken.str == "void" ) {
    // VOID Identifier function_definition_without_ID    
    token = GetToken(); // void
    type = token.str;
    token = GetToken(); // a Identifier
    if ( token.type != IDENT ) {
      ErrorHandler( token.str, 2 ); // Unexpect token
    } // if
    
    type = "void";
    name = token.str;
    Function_definition_without_ID( fdwiCorrect, type, name );
    
    correct = true;
  } // if
  else {
    // type_specifier Identifier function_definition_or_declarators
    Type_specifier( type_specifierCorrect, token );
    if ( !type_specifierCorrect ) {
      correct = false;
      return;
    } // if
    
    type = token.str;
    token = GetToken(); // a Identifier
    if ( token.type != IDENT ) {
      ErrorHandler( token.str, 2 ); // Unexpect token
      return;
    } // if

    name = token.str;
    Function_definition_or_declarators( fdodCorrect, type, name );
    correct = true;
  } // else
  
} // Definition()

void Type_specifier( bool & correct, Token & token ) {
  if ( gDEBUGMODE )
    cout << "Type_specifier\n";
  
  correct = true;
  PeekToken();
  
  if ( gUnDealToken.str == "int" ) {
    token = GetToken();
    token.type = INT;
    token.value_type = INT;
  } // if
  else if ( gUnDealToken.str == "char" ) {
    token = GetToken();
    token.type = CHAR;
    token.value_type = CHAR;
  } // else if
  else if ( gUnDealToken.str == "float" ) {
    token = GetToken();
    token.type = FLOAT;
    token.value_type = FLOAT;
  } // else if
  else if ( gUnDealToken.str == "string" ) {
    token = GetToken();
    token.type = STRING;
    token.value_type = STRING;
  } // else if
  else if ( gUnDealToken.str == "bool" ) {
    token = GetToken();
    token.type = BOOL;
    token.value_type = BOOL;
  } // else if
  else {
    token = gUnDealToken;
    correct = false;
    return;
  } // else
  
} // Type_specifier()

void Function_definition_or_declarators( bool & correct, string type, string name ) {
  // Function_definition_without_ID | Rest_of_declarators
  if ( gDEBUGMODE )
    cout << "Function_definition_or_declarators\n";
  Token token;
  bool fdwiCorrect = false, rodCorrect = false;
  Rest_of_declarators( rodCorrect, token, type, name, gInFunction, true );
  if ( !rodCorrect ) {
    Function_definition_without_ID( fdwiCorrect, type, name );
    if ( !fdwiCorrect ) {
      correct = false;
      ErrorHandler( token.str, 2 );
      return;
    } // if 
  } // if
  
  correct = true;
} // Function_definition_or_declarators()

void Rest_of_declarators( bool & correct, Token & token, 
                          string type, string name, bool global, bool print ) {
  // [ '[' Constant ']' ] { ',' Identifier [ '[' Constant ']' ] } ';'
  if ( gDEBUGMODE )
    cout << "Rest_of_declarators\n";
    
  Token temp;
  Variable var;
  bool defined = false;
  bool definedInFunc = false, definedInVar = false;
  correct = false;
  Array array;
  vector<Variable> vars;
  Initialize( token );
  
  var.type = String_to_enum( type );
  var.name = name;
  PeekToken();
  if ( gUnDealToken.str == "[" ) {
    temp = GetToken(); // '['
    token = GetToken(); // a Constant;
    if ( token.type != CONST ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    array.type = String_to_enum( type );
    array.name = name;
    array.length = atoi( token.str.c_str() );
    temp = GetToken(); // ']'
    if ( temp.str != "]" ) {
      ErrorHandler( temp.str, 2 );
    } // if 
    
    var.isArray = true;
    var.array.length = atoi( token.str.c_str() );
    var.array.name = var.name;
    var.array.type = var.type;
    var.array = array;
    vars.push_back( var );
  } // if
  else {
    Initialize( token );
    token.type = IDENT;
    token.value_type = String_to_enum( type );
    token.str = name;
    var.isArray = false;
    var.name = name;
    var.token = token;
    var.type = String_to_enum( type );
    vars.push_back( var );
  } // else
  
  PeekToken();
  while ( gUnDealToken.str == "," ) {
    InitializeVariable( var );
    var.type = String_to_enum( type );
    temp = GetToken(); // ','
    token = GetToken(); // a IDENT
    if ( token.type != IDENT ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    name = token.str;
    var.name = name;
    PeekToken();
    if ( gUnDealToken.str == "[" ) {
      token = GetToken(); // '['
      token = GetToken(); // a Constant;
      if ( token.type != CONST ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      array.type = array.type = String_to_enum( type );
      array.name = name;
      array.length = atoi( token.str.c_str() );
      token = GetToken(); // ']'
      if ( token.str != "]" ) {
        ErrorHandler( token.str, 2 );
      } // if 
      
      var.isArray = true;
      var.array = array;
      vars.push_back( var );
    } // if
    else {
      Initialize( token );
      token.type = IDENT;
      token.value_type = String_to_enum( type );
      token.str = name;
      var.isArray = false;
      var.name = name;
      var.token = token;
      var.type = String_to_enum( type );
      vars.push_back( var );
    } // else
    
    PeekToken() ;
  } // while
  
  PeekToken();
  if ( gUnDealToken.str == ";" ) { // 完全正確，回存
    correct = true; 
    temp = GetToken();
    int j = 0;
    for ( int i = 0; i < vars.size() ; i++ ) {
      definedInVar = false;
      definedInFunc = false;
      if ( !gInFunction && gCompoundTime == 0 ) {
        // global variable
        for ( j = 0, defined = false ; j < gGlobalVariable.size() && !defined ; j++ ) {
          if ( gGlobalVariable.at( j ).name == vars.at( i ).name ) {
            definedInVar = true; 
            defined = true;
          } // if
        } // for
        
        if ( !defined ) {
          for ( j = 0, defined = false ; j < gFunctionTable.size() && !defined ; j++ ) {
            if ( gFunctionTable.at( j ).name == vars.at( i ).name ) {
              definedInFunc = true; 
              defined = true;
            } // if
          } // for
        } // if
        
        
        if ( definedInVar ) {
          gGlobalVariable.erase( gGlobalVariable.begin() + j-1 );
          gGlobalVariable.push_back( vars.at( i ) );
          if ( print ) {
            if ( i == 0 ) {
              cout << "> ";
            } // if
            
            cout << "New definition of " << vars.at( i ).name << " entered ...\n";
            gLine = 0;
          } // if
          
        } // if
        else {
          gGlobalVariable.push_back( vars.at( i ) );
          if ( print ) {
            if ( i == 0 ) {
              cout << "> ";
            } // if
            
            cout << "Definition of " << vars.at( i ).name << " entered ...\n";
            gLine = 0;
          } // if
          
        } // else
        
        
      } // if
      else {
        // local variable
        gLocalVariable.push_back( vars.at( i ) );
      } // else 
      
    } // for
    
    if ( !gInFunction && gDEBUGMODE ) {
      // global variable
      for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
        cout << Enum_to_string( gGlobalVariable.at( i ).type ) << " ";
        if ( gGlobalVariable.at( i ).isArray ) {
          cout << gGlobalVariable.at( i ).name;
          cout << " [ " << gGlobalVariable.at( i ).array.length << " ]\n";
        } // if
        else {
          cout << gGlobalVariable.at( i ).name << "\n";
        } // else
        
      } // for
    } // if
    
    
    correct = true;
  } // if
  else {
    // temp = GetToken();
    correct = false;
    // ErrorHandler( temp.str, 2 );
    return;
  } // else
  
  correct = true;
} // Rest_of_declarators()

void Function_definition_without_ID( bool & correct,
                                     string functionType,
                                     string functionName ) {
  // '(' [ VOID | Formal_parameter_list ] ')' compouund_statement
  if ( gDEBUGMODE )
    cout << "Function_definition_without_ID\n";
  
  Token token;
  Function func;
  Parameter par;
  bool fplCorrect = false, csCorrect = false;
  bool definedInFunc = false, definedInVar = false;
  correct = false;
  
  func.type = String_to_enum( functionType );
  func.name = functionName;
  
  PeekToken();
  if ( gUnDealToken.str != "(" ) {
    // Not Function_definition_without_ID
    token = gUnDealToken;
    correct = false;
    ErrorHandler( token.str, 2 );
    return;
  } // if
  
  // gInFunction = true;
  token = GetToken(); // '('
  PeekToken();
  if ( gUnDealToken.str == "void" ) {
    Initialize( token );
    token = GetToken(); // "void"
    token.type = VOID;
    token.value_type = VOID;
    par.isArray = false;
    par.name = token.str;
    par.passAddr = false;
    par.token = token;
    par.type = VOID;
    func.parameter.push_back( par );
  } // if
  else {
    Formal_parameter_list( fplCorrect, func );
  } // else
  
  PeekToken();
  if ( gUnDealToken.str != ")" ) {
    token = GetToken();
    ErrorHandler( token.str, 2 );
  } // if
  
  token = GetToken(); // ')'
  gLogStatement = true;
  gTempStatements.clear();
  Compound_Statement( csCorrect, func, token );
  func.statements = gTempStatements;
  gLogStatement = false;
  if ( !csCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  // 此處function定義完成
  // 找看看有沒有同id的variable，
  // 有則new defined、pop、push 
  // 無則直接push
  bool defined = false;
  int i = 0;
  while ( i < gFunctionTable.size() && !defined ) {
    if ( functionName == gFunctionTable.at( i ).name ) {
      defined = true;
      definedInFunc = true;
    } // if
    else {
      i++;
    } // else
   
  } // while
  
  if ( !defined ) {
    i = 0;
    while ( i < gGlobalVariable.size() && !defined ) {
      if ( functionName == gGlobalVariable.at( i ).name ) {
        defined = true;
        definedInVar = true;
      } // if
      else {
        i++;
      } // else
     
    } // while
  } // if
  
  if ( definedInFunc ) {
    // new defined、pop、push
    gFunctionTable.erase( gFunctionTable.begin() + i );
    gFunctionTable.push_back( func );
    cout << "> New definition of " << func.name << "() entered ...\n";
    gLine = 0;
  } // if
  else {
    // 直接push
    gFunctionTable.push_back( func );
    cout << "> Definition of " << func.name << "() entered ...\n";
    gLine = 0;
  } // else
  
  if ( gDEBUGMODE ) {
    for ( i = 0 ; i < gFunctionTable.size() ; i++ ) {
      cout << Enum_to_string( gFunctionTable.at( i ).type ) << " ";
      cout << gFunctionTable.at( i ).name << "( ";
      for ( int j = 0; j < gFunctionTable.at( i ).parameter.size() ; j++ ) {
        // cout << gFunctionTable.at(i).parameter.at(j).type << " ";
        if ( gFunctionTable.at( i ).parameter.at( j ).name == "void" ) {
          cout << "void";
        } // if
        else {
          cout << Enum_to_string( gFunctionTable.at( i ).parameter.at( j ).type ) << " ";
          if ( gFunctionTable.at( i ).parameter.at( j ).passAddr ) {
            cout << "& ";
          } // if
          
          cout << gFunctionTable.at( i ).parameter.at( j ).name;
        } // else
        
        if ( gFunctionTable.at( i ).parameter.at( j ).isArray ) {
          cout << "[ ";
          cout << gFunctionTable.at( i ).parameter.at( j ).array.length;
          cout << " ]";
        } // if
        
        if ( j < gFunctionTable.at( i ).parameter.size()-1 ) {
          cout << ", ";
        } // if
        else {
          cout << " ";
        } // else
        
      } // for
      
      cout << ") ";
      for ( int j = 0; j < gFunctionTable.at( i ).statements.size() ; j++ ) {
        string toPrint = gFunctionTable.at( i ).statements.at( j ).str;
        if ( toPrint == "{" ) {
          cout << toPrint << "\n";
          if ( j != gFunctionTable.at( i ).statements.size()-2 ) {
            cout << "  ";
          } // if
          
        } // if
        else if ( toPrint == "}" ) {
          cout << toPrint << "\n";
        } // else if
        else if ( toPrint == ";" ) {
          cout << toPrint << "\n";
          if ( j != gFunctionTable.at( i ).statements.size()-2 ) {
            // 當進入compound statement/ if...else/while/do...while時要多印空白 
            cout << "  ";
          } // if
          
        } // else if
        else {
          cout << toPrint << " ";
        } // else
        
      } // for
      
    } // for
    
    cout << "Local Variable :\n";
    for ( int i = 0; i < gLocalVariable.size() ; i++ ) {
      cout << Enum_to_string( gLocalVariable.at( i ).type ) << " " << gLocalVariable.at( i ).name << "\n";
    } // for
  } // if
  
  
  correct = true;
  gLocalVariable.clear();
  // gInFunction = false;
} // Function_definition_without_ID()

void Formal_parameter_list( bool & correct, Function & func ) {
  // type_specifier [ '&' ] Identifier [ '[' Constant ']' ]
  // { ',' type_specifier [ '&' ] Identifier [ '[' Constant ']' ] }
  if ( gDEBUGMODE )
    cout << "Formal_parameter_list\n";
 
  Token token;
  Token temp;
  Variable var;
  Array array;
  Parameter par;
  string type = "", name = "";
  bool type_specifierCorrect = false;
  Type_specifier( type_specifierCorrect, token );
  if ( !type_specifierCorrect ) {
    // Not Formal_parameter_list
    correct = false;
    return;
  } // if
  
  var.type = token.type;
  par.type = token.type;
  PeekToken();
  if ( gUnDealToken.str == "&" ) {
    temp = GetToken(); // '&'
    par.passAddr = true;
  } // if
  else {
    par.passAddr = false;
  } // else
  
  token = GetToken(); // a ID
  if ( token.type != IDENT ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  var.name = token.str;
  par.name = token.str;
  PeekToken();
  if ( gUnDealToken.str == "[" ) {
    par.isArray = true;
    var.isArray = true;
    temp = GetToken(); // '['
    token = GetToken(); // a Constant;
    if ( token.type != CONST ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    array.type = String_to_enum( type );
    array.name = token.str;
    array.length = atoi( token.str.c_str() );
    temp = GetToken(); // ']'
    if ( temp.str != "]" ) {
      ErrorHandler( temp.str, 2 );
    } // if 
    
    par.array = array;
    var.array = array;
    func.parameter.push_back( par );
    gLocalVariable.push_back( var );
  } // if
  else {
    var.isArray = false;
    var.name = token.str;
    var.token = token;
    par.isArray = false;
    par.name = token.str;
    par.token = token;
    func.parameter.push_back( par );
    gLocalVariable.push_back( var );
  } // else
  
  PeekToken();
  while ( gUnDealToken.str == "," ) {
    InitializeParameter( par );
    InitializeVariable( var );
    temp = GetToken(); // ','
    Type_specifier( type_specifierCorrect, token );
    if ( !type_specifierCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    par.type = token.type;
    var.type = token.type;
    PeekToken();
    if ( gUnDealToken.str == "&" ) {
      temp = GetToken(); // '&'
      par.passAddr = true;
    } // if
    else {
      par.passAddr = false;
    } // else
    
    token = GetToken(); // a IDENT
    if ( token.type != IDENT ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    par.name = token.str;
    var.name = token.str;
    PeekToken();
    if ( gUnDealToken.str == "[" ) {
      temp = GetToken(); // '['
      temp = GetToken(); // a Constant;
      if ( temp.type != CONST ) {
        ErrorHandler( temp.str, 2 );
      } // if
      
      var.isArray = true;
      par.isArray = true;
      par.array.type = par.type;
      par.array.name = token.str;
      par.array.length = atoi( temp.str.c_str() );
      temp = GetToken(); // ']'
      if ( temp.str != "]" ) {
        ErrorHandler( temp.str, 2 );
      } // if 
      
      var.array = array;
      gLocalVariable.push_back( var );
      func.parameter.push_back( par );
    } // if
    else {
      // Initialize( token );
      par.isArray = false;
      par.name = token.str;
      par.token = token;
      var.isArray = false;
      var.name = token.str;
      var.token = token;
      gLocalVariable.push_back( var );
      func.parameter.push_back( par );
    } // else
    
    PeekToken() ;
  } // while
  
  correct = true;
} // Formal_parameter_list()

void Compound_Statement( bool & correct, Function & func, Token & token ) {
  // '{' { declaration | statement } '}'
  if ( gDEBUGMODE )
    cout << "Compound_Statement\n";
  
  bool declarationCorrect = false, statementCorrect = false;
  Token temp;
  PeekToken();
  if ( gUnDealToken.str != "{" ) {
    // Not Compound_Statement
    token = gUnDealToken;
    correct = false;
    return;
  } // if
  
  gCompoundTime++;
  gInFunction = true; // Local variable開始 
  temp = GetToken(); // '{'
  Declaration( declarationCorrect, false );
  if ( !declarationCorrect ) {
    Statement( statementCorrect, token, false ) ;
    if ( !statementCorrect ) {
      temp = GetToken(); // '}'
      if ( temp.str != "}" ) {
        ErrorHandler( temp.str, 2 );
      } // if
      
      gInFunction = false; // Local variable結束 
      gCompoundTime--;
      correct = true;
      return;
    } // if
  } // if
  
  while ( declarationCorrect || statementCorrect ) {
    Declaration( declarationCorrect, false );
    if ( !declarationCorrect ) {
      Statement( statementCorrect, token, false ) ;
    } // if
  } // while
  
  
  temp = GetToken(); // '}'
  if ( temp.str != "}" ) {
    ErrorHandler( temp.str, 2 );
  } // if
  
  correct = true;
  gCompoundTime--;
  gInFunction = false; // local variable結束 
} // Compound_Statement()

void Declaration( bool & correct, bool print ) {
  // type_specifier Identifier rest_of_declarators
  if ( gDEBUGMODE )
    cout << "Declaration\n";
  
  Token token;
  string type, name;
  bool type_specifierCorrect = false;
  bool rodCorrect = false;
  Type_specifier( type_specifierCorrect, token );
  if ( !type_specifierCorrect ) {
    // Not Declaration
    correct = false;
    return;
  } // if 

  type = token.str;
  token = GetToken(); // a ID
  if ( token.type != IDENT ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  name = token.str;
  Rest_of_declarators( rodCorrect, token, type, name, gInFunction, print );
  if ( !rodCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
    
  correct = true;
} // Declaration()

void Statement( bool & correct, Token & token, bool print ) {
  // ';' // the null statement
  // | expression ';' /* expression here should not be empty */
  // | RETURN [ expression ] ';'
  // | compound_statement
  // | IF '(' expression ')' statement [ ELSE statement ]
  // | WHILE '(' expression ')' statement
  // | DO statement WHILE '(' expression ')' ';'
 
  if ( gDEBUGMODE )
    cout << "Statement\n";
    
  bool csCorrect = false, expressionCorrect = false;
  bool sCorrect = false, defined = false;
  Token temp;
  Function func;
  correct = true;
  PeekToken();
  if ( gUnDealToken.str == ";" ) {
    token = GetToken();
    if ( print ) {
      gLine = 0;
      cout << "> Statement executed ...\n";
    } // if
    
    return;
  } // if
  else if ( gUnDealToken.str == "return" ) {
    temp = GetToken();
    Expression( expressionCorrect, token );
    temp = GetToken();
    if ( temp.str != ";" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    if ( print ) {
      gLine = 0;
      cout << "> Statement executed ...\n";
    } // if
      
  } // else if
  else if ( gUnDealToken.str == "if" ) {
    temp = GetToken();
    temp = GetToken();
    if ( temp.str != "(" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    Expression( expressionCorrect, token );
    if ( !expressionCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    temp = GetToken();
    if ( temp.str != ")" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    Statement( sCorrect, token, false );
    if ( !sCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    PeekToken();
    if ( gUnDealToken.str == "else" ) {
      temp = GetToken();
      Statement( sCorrect, token, false );
      if ( !sCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
    } // if
    
    if ( print ) {
      gLine = 0;
      cout << "> Statement executed ...\n";
    } // if
    
  } // else if
  else if ( gUnDealToken.str == "while" ) {
    temp = GetToken();
    temp = GetToken();
    if ( temp.str != "(" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    Expression( expressionCorrect, token );
    if ( !expressionCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    temp = GetToken();
    if ( temp.str != ")" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    Statement( sCorrect, token, false );
    if ( !sCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    if ( print ) {
      gLine = 0;
      cout << "> Statement executed ...\n";
    } // if
      
  } // else if
  else if ( gUnDealToken.str == "do" ) {
    temp = GetToken();
    Statement( sCorrect, token, false );
    if ( !sCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    temp = GetToken();
    if ( temp.str != "while" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    temp = GetToken();
    if ( temp.str != "(" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    Expression( expressionCorrect, token );
    if ( !expressionCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    temp = GetToken();
    if ( temp.str != ")" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    temp = GetToken();
    if ( temp.str != ";" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    if ( print ) {
      gLine = 0;
      cout << "> Statement executed ...\n";
    } // if
    
  } // else if
  else if ( gUnDealToken.str == "cout" ) {
    // COUT '<<' { COUT '<<' } <expression> { '<<' { COUT '<<' } <expression> } ';'
    temp = GetToken();
    PeekToken();
    if ( gUnDealToken.str != "<<" ) {
      temp = GetToken();
      ErrorHandler( temp.str, 2 );
    } // if
    
    temp = GetToken();
    PeekToken();
    while ( gUnDealToken.str == "cout" ) {
      temp = GetToken();
      PeekToken();
      if ( gUnDealToken.str != "<<" ) {
        temp = GetToken();
        ErrorHandler( temp.str, 2 );
      } // if
      
      temp = GetToken();
      PeekToken();
    } // while
    
    Expression( expressionCorrect, token );
    if ( !expressionCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    if ( gDEBUGMODE ) {
      cout << "\nANS in Final : " ;
      PrintValue( token ) ;
      cout << "\n";
    } // if
  
    PrintValue( token ) ;
    
    PeekToken();
    while ( gUnDealToken.str == "<<" ) {
      temp = GetToken();
      PeekToken();
      while ( gUnDealToken.str == "cout" ) {
        temp = GetToken();
        PeekToken();
        if ( gUnDealToken.str != "<<" ) {
          temp = GetToken();
          ErrorHandler( temp.str, 2 );
        } // if
        
        temp = GetToken();
        PeekToken();
      } // while
      
      Expression( expressionCorrect, token );
      if ( !expressionCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      if ( gDEBUGMODE ) {
        cout << "\nANS in Final : " ;
        PrintValue( token ) ;
        cout << "\n";
      } // if
      
      PrintValue( token ) ;
      
      PeekToken();
    } // while
    
    PeekToken();
    if ( gUnDealToken.str != ";" ) {
      temp = GetToken();
      ErrorHandler( temp.str, 2 );
    } // if
    
    temp = GetToken();
    
    if ( print ) {
      gLine = 0;
      cout << "Statement executed ...\n";
    } // if
    
  } // else if
  else if ( gUnDealToken.str == "cin" ) {
    // cin '>>' IDENT [ '[' CONSTANT ']' ] { '>>' IDENT [ '[' CONSTANT ']' ] } ';'
    temp = GetToken();
    PeekToken();
    if ( gUnDealToken.str != ">>" ) {
      temp = GetToken();
      ErrorHandler( temp.str, 2 );
    } // if
    
    temp = GetToken();
    PeekToken();
    if ( gUnDealToken.type != IDENT ) {
      temp = GetToken();
      ErrorHandler( temp.str, 2 );
    } // if
    
    temp = GetToken();
    // 找出是否宣告
    for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
      if ( temp.str == gGlobalVariable.at( i ).name ) {
        defined = true; 
      } // if
      
    } // for
    
    if ( !defined ) {
      for ( int i = 0; i < gLocalVariable.size() ; i++ ) {
        if ( temp.str == gLocalVariable.at( i ).name ) {
          defined = true; 
        } // if
        
      } // for
    } // if
    
    if ( !defined ) {
      ErrorHandler( temp.str, 3 );
    } // if
    
    PeekToken();
    if ( gUnDealToken.str == "[" ) {
      temp = GetToken();
      Expression( expressionCorrect, token );
      if ( !expressionCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      PeekToken();
      if ( gUnDealToken.str != "]" ) {
        temp = GetToken();
        ErrorHandler( temp.str, 2 );
      } // if
      
      temp = GetToken();
      
    } // if
    
    PeekToken();
    while ( gUnDealToken.str == ">>" ) {
      temp = GetToken();
      PeekToken();
      if ( gUnDealToken.type != IDENT ) {
        temp = GetToken();
        ErrorHandler( temp.str, 2 );
      } // if
      
      temp = GetToken();
      defined = false;
      for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
        if ( temp.str == gGlobalVariable.at( i ).name ) {
          defined = true; 
        } // if
        
      } // for
      
      if ( !defined ) {
        for ( int i = 0; i < gLocalVariable.size() ; i++ ) {
          if ( temp.str == gLocalVariable.at( i ).name ) {
            defined = true; 
          } // if
          
        } // for
      } // if
      
      if ( !defined ) {
        ErrorHandler( temp.str, 3 );
      } // if
      
      PeekToken();
      if ( gUnDealToken.str == "[" ) {
        temp = GetToken();
        Expression( expressionCorrect, token );
        if ( !expressionCorrect ) {
          ErrorHandler( token.str, 2 );
        } // if
        
        PeekToken();
        if ( gUnDealToken.str != "]" ) {
          temp = GetToken();
          ErrorHandler( temp.str, 2 );
        } // if
        
        temp = GetToken();
        
      } // if
      
      PeekToken();
    } // while
    
    PeekToken();
    if ( gUnDealToken.str != ";" ) {
      temp = GetToken();
      ErrorHandler( temp.str, 2 );
    } // if
    
    temp = GetToken();
    
    if ( print ) {
      gLine = 0;
      cout << "> Statement executed ...\n";
    } // if
    
  } // else if
  else {
    Compound_Statement( csCorrect, func, token );
    if ( !csCorrect ) {
      Expression( expressionCorrect, token );
      if ( !expressionCorrect ) {
        correct = false;
        return;
      } // if
      
      token = GetToken();
      if ( token.str != ";" ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      FunctionHandler( gFunctionName, gParameter, print );
      if ( print ) {
        gLine = 0;
        cout << "Statement executed ...\n";
      } // if
      
    } // if
    else {
      if ( print ) {
        gLine = 0;
        cout << "> Statement executed ...\n";
      } // if
        
    } // else
 
  } // else
  
  
} // Statement()

void Expression( bool & correct, Token & token ) {
  // basic_expression { ',' basic_expression }
  if ( gDEBUGMODE )
    cout << "Expression\n";
  bool beCorrect = false;
  Basic_expression( beCorrect, token );
  if ( !beCorrect ) {
    correct = false;
    return;
  } // if

  PeekToken();
  while ( gUnDealToken.str == "," ) {
    token = GetToken(); 
    Basic_expression( beCorrect, token );
    if ( !beCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    PeekToken();
  } // while
  
  correct = true;
} // Expression()


void Basic_expression( bool & correct, Token & token ) {
  // Identifier rest_of_Identifier_started_basic_exp
  // | ( PP | MM ) Identifier rest_of_PPMM_Identifier_started_basic_exp
  // | sign { sign } signed_unary_exp romce_and_romloe
  // | ( Constant | '(' expression ')' ) romce_and_romloe
  if ( gDEBUGMODE )
    cout << "Basic_expression\n";
  
  bool roisbCorrect = false;
  bool ropisbeCorrect = false;
  bool sueCorrect = false;
  bool rarCorrect = false;
  bool expCorrect = false;
  bool defined = false, sCorrect = false;
  Token temp, sign;
  int signCount1 = 0, signCount2 = 0, signCount3 = 0;
  correct = false;
  PeekToken();
  if ( gUnDealToken.type == IDENT ) {
    // 此處取得第一個IDENT 
    token = GetToken();
    if ( token.str == "else" ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    // 在此檢查是否define過
    // if gInFunction ==> 找Local和Global
    // else 找 Global 和 Function
    if ( gCompoundTime > 0 ) {
      for ( int i = 0; i < gLocalVariable.size() ; i++ ) {
        if ( token.str == gLocalVariable.at( i ).name ) {
          token = gLocalVariable.at( i ).token;
          defined = true;
        } // if
      } // for
      
      if ( !defined ) {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( token.str == gGlobalVariable.at( i ).name ) {
            token = gGlobalVariable.at( i ).token;
            defined = true;
          } // if
        } // for
      } // if
      
      if ( !defined ) {
        for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
          if ( token.str == gFunctionTable.at( i ).name ) {
            defined = true;
          } // if
        } // for
      } // if
      
      if ( !defined ) {
        ErrorHandler( token.str, 3 );
      } // if
      
    } // if
    else {
      for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
        if ( token.str == gGlobalVariable.at( i ).name ) {
          token = gGlobalVariable.at( i ).token;
          defined = true;
        } // if
      } // for
      
      if ( !defined ) {
        for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
          if ( token.str == gFunctionTable.at( i ).name ) {
            defined = true;
          } // if
        } // for
      } // if
      
      if ( token.str == "ListAllVariables" ||
           token.str == "ListAllFunctions" ||
           token.str == "ListVariable" ||
           token.str == "ListFunction" ||
           token.str == "Done" ) {
        gFunctionName = token.str;
        defined = true;    
      } // if
      
      if ( !defined ) {
        ErrorHandler( token.str, 3 );
      } // if
      
    } // else 
    
    Rest_of_ID_started_basic_exp( roisbCorrect, token );
    if ( !roisbCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    correct = true;
    return;
  } // if
  else if ( gUnDealToken.str == "++" || gUnDealToken.str == "--" ) {
    token = GetToken();
    if ( token.str == "++" ) {
      token = GetToken();
      if ( token.type != IDENT ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      // 在此檢查是否define過
      // if gInFunction ==> 找Local和Global
      // else 找 Global 和 Function
      if ( gCompoundTime > 0 ) {
        for ( int i = 0; i < gLocalVariable.size() ; i++ ) {
          if ( token.str == gLocalVariable.at( i ).name ) {
            if ( !gLocalVariable.at( i ).isArray ) {
              gLocalVariable.at( i ).token.value += 1;
              token = gLocalVariable.at( i ).token;
            } // if
            
            defined = true;
          } // if
        } // for
        
        if ( !defined ) {
          for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
            if ( token.str == gGlobalVariable.at( i ).name ) {
              if ( !gGlobalVariable.at( i ).isArray ) {
                gGlobalVariable.at( i ).token.value += 1;
                token = gGlobalVariable.at( i ).token;
              } // if
             
              defined = true;
            } // if
          } // for
        } // if
        
        if ( !defined ) {
          ErrorHandler( token.str, 3 );
        } // if
        
      } // if
      else {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( token.str == gGlobalVariable.at( i ).name ) {
            if ( !gGlobalVariable.at( i ).isArray ) {
              gGlobalVariable.at( i ).token.value += 1;
              token = gGlobalVariable.at( i ).token;
            } // if
            
            defined = true;
          } // if
        } // for
        
        if ( !defined ) {
          for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
            if ( token.str == gFunctionTable.at( i ).name ) {
              defined = true;
            } // if
          } // for
        } // if
        
        if ( !defined ) {
          ErrorHandler( token.str, 3 );
        } // if
        
      } // else 
      
      Rest_of_PPMM_ID_started_basic_exp( ropisbeCorrect, token );
      if ( !ropisbeCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
    } // if
    else {
      token = GetToken();
      if ( token.type != IDENT ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      // 在此檢查是否define過
      // if gInFunction ==> 找Local和Global
      // else 找 Global 和 Function
      if ( gCompoundTime > 0 ) {
        for ( int i = 0; i < gLocalVariable.size() ; i++ ) {
          if ( token.str == gLocalVariable.at( i ).name ) {
            if ( !gLocalVariable.at( i ).isArray ) {
              gLocalVariable.at( i ).token.value -= 1;
              token = gLocalVariable.at( i ).token;
            } // if
            
            defined = true;
          } // if
        } // for
        
        if ( !defined ) {
          for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
            if ( token.str == gGlobalVariable.at( i ).name ) {
              if ( !gGlobalVariable.at( i ).isArray ) {
                gGlobalVariable.at( i ).token.value -= 1;
                token = gGlobalVariable.at( i ).token;
              } // if
              
              defined = true;
            } // if
          } // for
        } // if
        
        if ( !defined ) {
          ErrorHandler( token.str, 3 );
        } // if
        
      } // if
      else {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( token.str == gGlobalVariable.at( i ).name ) {
            if ( !gGlobalVariable.at( i ).isArray ) {
              gGlobalVariable.at( i ).token.value -= 1;
              token = gGlobalVariable.at( i ).token;
            } // if
            
            defined = true;
          } // if
        } // for
        
        if ( !defined ) {
          for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
            if ( token.str == gFunctionTable.at( i ).name ) {
              defined = true;
            } // if
          } // for
        } // if
        
        if ( !defined ) {
          ErrorHandler( token.str, 3 );
        } // if
        
      } // else 
      
      Rest_of_PPMM_ID_started_basic_exp( ropisbeCorrect, token );
      if ( !ropisbeCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
    } // else 
    
  } // else if
  else if ( gUnDealToken.type == CONST || gUnDealToken.str == "(" ) {
    token = GetToken();
    if ( token.type == CONST ) {
      if ( token.value_type == INT ) {
        token.value = atoi( token.str.c_str() );
      } // if
      else if ( token.value_type == INT ) {
        token.value = atof( token.str.c_str() );
      } // else if
      
      Romce_and_romloe( rarCorrect, token );
      if ( !rarCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
    } // if
    else {
      Expression( expCorrect, token );
      if ( !expCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      temp = GetToken();
      if ( temp.str != ")" ) {
        ErrorHandler( temp.str, 2 );
      } // if
      
      Romce_and_romloe( rarCorrect, token );
      if ( !rarCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
    } // else
    
  } // else if
  else {
    // | sign { sign } signed_unary_exp romce_and_romloe
    Sign( sCorrect, sign );
    if ( !sCorrect ) {
      correct = false;
      token = gUnDealToken;
      return;
    } // if
    
    if ( sign.str == "!" ) {
      signCount3++;
    } // if
    else if ( sign.str == "-" ) {
      signCount2++;
    } // else if
    else {
      signCount1++;
    } // else
    
    while ( sCorrect ) {
      Sign( sCorrect, sign );
      if ( sCorrect ) {
        if ( sign.str == "!" ) {
          signCount3++;
        } // if
        else if ( sign.str == "-" ) {
          signCount2++;
        } // else if
        else {
          signCount1++;
        } // else
        
      } // if
      
    } // while
    
    Signed_unary_exp( sueCorrect, token );
    if ( !sueCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    if ( signCount3 != 0 ) {
      if ( signCount3 % 2 == 0 ) {
        if ( token.value != 0 ) {
          token.value = 0;
        } // if
        else {
          token.value = 1;
        } // else
        
      } // if
      else {
        if ( token.value != 0 ) {
          token.value = 1;
        } // if
        else {
          token.value = 0;
        } // else
        
      } // else
      
    } // if
    
    if ( signCount2 % 2 != 0 ) {
      token.value = 0 - token.value;
    } // if
    
    Romce_and_romloe( rarCorrect, token );
    if ( !rarCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
  } // else
  
  correct = true;
} // Basic_expression()

void Rest_of_ID_started_basic_exp( bool & correct, Token & token ) {
  // [ '[' expression ']' ]
  // ( assignment_operator basic_expression | [ PP | MM ] romce_and_romloe )
  // | '(' [ actual_parameter_list ] ')' romce_and_romloe
  if ( gDEBUGMODE )
    cout << "Rest_of_ID_started_basic_exp\n";
  
  correct = false;
  PeekToken();
  Token temp, token1, temp1;
  bool aoCorrect = false, rarCorrect = false;
  bool beCorrect = false, aplCorrect = false;
  bool expCorrect = false;
  
  temp1 = token;
  if ( gUnDealToken.str == "(" ) {
    temp = GetToken();
    Actual_parameter_list( aplCorrect, token1 );
    temp = GetToken();
    if ( temp.str != ")" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    // function call
    gParameter = token1.str;
    
    Romce_and_romloe( rarCorrect, token );
    if ( !rarCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    correct = true;
  } // if
  else {
    if ( gUnDealToken.str == "[" ) {
      temp = GetToken(); 
      Expression( expCorrect, token1 );
      if ( !expCorrect ) {
        ErrorHandler( token1.str, 2 );
      } // if
      
      temp = GetToken();
      if ( temp.str != "]" ) {
        ErrorHandler( temp.str, 2 );
      } // if
      
    } // if
    
    Assignment_operator( aoCorrect, temp );
    if ( aoCorrect ) {
      Basic_expression( beCorrect, token1 );
      if ( !beCorrect ) {
        ErrorHandler( token1.str, 2 );
      } // if
      
      if ( temp.str == "=" ) {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( temp1.str == gGlobalVariable.at( i ).name ) {
            token.value = token1.value;
            token.value_type = token1.value_type;
            gGlobalVariable.at( i ).token = token;
          } // if
          
        } // for
        
      } // if
      else if ( temp.str == "+=" ) {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( temp1.str == gGlobalVariable.at( i ).name ) {
            gGlobalVariable.at( i ).token.value += token1.value;
            token = gGlobalVariable.at( i ).token;
          } // if
          
        } // for
        
      } // else if
      else if ( temp.str == "-=" ) {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( temp1.str == gGlobalVariable.at( i ).name ) {
            gGlobalVariable.at( i ).token.value -= token1.value;
            token = gGlobalVariable.at( i ).token;
          } // if
          
        } // for
        
      } // else if
      else if ( temp.str == "*=" ) {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( temp1.str == gGlobalVariable.at( i ).name ) {
            gGlobalVariable.at( i ).token.value *= token1.value;
            token = gGlobalVariable.at( i ).token;
          } // if
          
        } // for
        
      } // else if
      else if ( temp.str == "/=" ) {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( temp1.str == gGlobalVariable.at( i ).name ) {
            gGlobalVariable.at( i ).token.value /= token1.value;
            token = gGlobalVariable.at( i ).token;
          } // if
          
        } // for
        
      } // else if
      else if ( temp.str == "%=" ) {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( temp1.str == gGlobalVariable.at( i ).name ) {
            gGlobalVariable.at( i ).token.value = int( gGlobalVariable.at( i ).token.value ) % int( token1.value );
            token = gGlobalVariable.at( i ).token;
          } // if
          
        } // for
        
      } // else if
      
    } // if
    else {
      PeekToken();
      if ( gUnDealToken.str == "++" || gUnDealToken.str == "--" ) {
        temp = GetToken();
      } // if
      
      Romce_and_romloe( rarCorrect, token );
      
      if ( !rarCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if

    } // else
   
    correct = true;
  } // else
  
} // Rest_of_ID_started_basic_exp()

void Rest_of_PPMM_ID_started_basic_exp( bool & correct, Token & token ) {
  // [ '[' expression ']' ] romce_and_romloe
  if ( gDEBUGMODE )
    cout << "Rest_of_PPMM_ID_started_basic_exp\n";
  
  Token temp;
  correct = false;
  bool expCorrect = false, rarCorrect = false; 
  PeekToken();
  if ( gUnDealToken.str == "[" ) {
    temp = GetToken();
    Expression( expCorrect, token );
    if ( !expCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    temp = GetToken();
    if ( temp.str != "]" ) {
      ErrorHandler( token.str, 2 );
    } // if
  } // if
  
  Romce_and_romloe( rarCorrect, token );
  if ( !rarCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  correct = true;
} // Rest_of_PPMM_ID_started_basic_exp()

void Sign( bool & correct, Token & token ) {
  if ( gDEBUGMODE )
    cout << "Sign\n";
 
  //  '+' | '-' | '!'
  PeekToken();
  if ( gUnDealToken.str == "+" ) {
    token = GetToken();
  } // if
  else if ( gUnDealToken.str == "-" ) {
    token = GetToken();
  } // else if
  else if ( gUnDealToken.str == "!" ) {
    token = GetToken();
  } // else if
  else {
    token = gUnDealToken;
    correct = false;
    return;
  } // else
  
  correct = true;
} // Sign()

void Actual_parameter_list( bool & correct, Token & token ) {
  // basic_expression { ',' basic_expression }
  if ( gDEBUGMODE )
    cout << "Actual_parameter_list\n";
    
  bool beCorrect = false;
  Basic_expression( beCorrect, token );
  if ( !beCorrect ) {
    correct = false;
    return;
  } // if
  
  
  PeekToken();
  while ( gUnDealToken.str == "," ) {
    token = GetToken(); 
    Basic_expression( beCorrect, token );
    if ( !beCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    PeekToken();
  } // while
  
  correct = true;
  
} // Actual_parameter_list()

void Assignment_operator( bool & correct, Token & token ) {
  //  '=' | TE | DE | RE | PE | ME
  if ( gDEBUGMODE )
    cout << "Assignment_operator\n";
  
  PeekToken();
  if ( gUnDealToken.str == "=" ) {
    token = GetToken();
  } // if
  else if ( gUnDealToken.str == "*=" ) {
    token = GetToken();
  } // else if
  else if ( gUnDealToken.str == "/=" ) {
    token = GetToken();
  } // else if
  else if ( gUnDealToken.str == "%=" ) {
    token = GetToken();
  } // else if
  else if ( gUnDealToken.str == "+=" ) {
    token = GetToken();
  } // else if
  else if ( gUnDealToken.str == "-=" ) {
    token = GetToken();
  } // else if
  else {
    token = gUnDealToken;
    correct = false;
    return;
    
  } // else

  correct = true;
} // Assignment_operator()

void Romce_and_romloe( bool & correct, Token & token ) {
  // Rest_of_maybe_logical_OR_exp [ '?' basic_expression ':' basic_expression ]
  if ( gDEBUGMODE )
    cout << "Romce_and_romloe\n";
  
  bool romloeCorrect = false;
  bool beCorrect = false;
  correct = false;
  Token temp, token1, token2;
  Rest_of_maybe_logical_OR_exp( romloeCorrect, token );
  if ( !romloeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  if ( gUnDealToken.str == "?" ) {
    temp = GetToken();
    Basic_expression( beCorrect, token1 );
    if ( !beCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    temp = GetToken();
    if ( temp.str != ":" ) {
      ErrorHandler( temp.str, 2 );
    } // if
    
    Basic_expression( beCorrect, token2 );
    if ( !beCorrect ) {
      ErrorHandler( token2.str, 2 );
    } // if
    
    if ( token.value != 0 ) {
      token = token1;
    } // if
    else {
      token = token2;
    } // else
    
  } // if

  if ( gDEBUGMODE ) {
    cout << "\nANS in Rest_of_maybe_logical_OR_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;  
} // Romce_and_romloe()

void Rest_of_maybe_logical_OR_exp( bool & correct, Token & token ) {
  // rest_of_maybe_logical_AND_exp { OR maybe_logical_AND_exp }
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_logical_OR_exp\n";
  
  bool romlaeCorrect = false;
  bool mlaeCorrect = false;
  correct = false;
  Token temp, token1;
  Rest_of_maybe_logical_AND_exp( romlaeCorrect, token );
  if ( !romlaeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "||" ) {
    temp = GetToken();
    Maybe_logical_AND_exp( mlaeCorrect, token1 );
    if ( !mlaeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( token.value != 0 || token1.value != 0 ) {
      token.value = 1;
      token.value_type = BOOL;
    } // if
    else {
      token.value = 0;
      token.value_type = BOOL;
    } // else
    
    PeekToken();
  } // while

  if ( gDEBUGMODE ) {
    cout << "\nANS in Rest_of_maybe_logical_OR_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Rest_of_maybe_logical_OR_exp()

void Maybe_logical_AND_exp( bool & correct, Token & token ) {
  // maybe_bit_OR_exp { AND maybe_bit_OR_exp }
  if ( gDEBUGMODE )
    cout << "Maybe_logical_AND_exp\n";
  
  correct = false;
  bool mboeCorrect = false ;
  Token temp, token1;
  Maybe_bit_OR_exp( mboeCorrect, token );
  if ( !mboeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "&&" ) {
    temp = GetToken();
    Maybe_bit_OR_exp( mboeCorrect, token1 );
    if ( !mboeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( token.value == 0 || token1.value == 0 ) {
      token.value = 0;
      token.value_type = BOOL;
    } // if
    else {
      token.value = 1;
      token.value_type = BOOL;
    } // else
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Maybe_logical_AND_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Maybe_logical_AND_exp()

void Rest_of_maybe_logical_AND_exp( bool & correct, Token & token ) {
  // rest_of_maybe_bit_OR_exp { AND maybe_bit_OR_exp }
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_logical_AND_exp\n";
  
  correct = false;
  bool romboeCorrect = false, mboeCorrect = false;
  Token temp, token1;
  Rest_of_maybe_bit_OR_exp( romboeCorrect, token );
  if ( !romboeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "&&" ) {
    temp = GetToken();
    Maybe_bit_OR_exp( mboeCorrect, token1 );
    if ( !mboeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( token.value == 0 || token1.value == 0 ) {
      token.value = 0;
      token.value_type = BOOL;
    } // if
    else {
      token.value = 1;
      token.value_type = BOOL;
    } // else
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Rest_of_maybe_logical_AND_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Rest_of_maybe_logical_AND_exp()

void Maybe_bit_OR_exp( bool & correct, Token & token ) {
  // maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }
  if ( gDEBUGMODE )
    cout << "Maybe_bit_OR_exp\n";
  
  bool mbeoeCorrect = false;
  correct = false;
  Token temp, token1;
  Maybe_bit_ex_OR_exp( mbeoeCorrect, token );
  if ( !mbeoeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "|" ) {
    temp = GetToken();
    Maybe_bit_ex_OR_exp( mbeoeCorrect, token1 );
    if ( !mbeoeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    token.value = int( token.value ) | int( token1.value );
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Maybe_bit_OR_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
 
  correct = true;
} // Maybe_bit_OR_exp()

void Rest_of_maybe_bit_OR_exp( bool & correct, Token & token ) {
  // rest_of_maybe_bit_ex_OR_exp { '|' maybe_bit_ex_OR_exp }
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_bit_OR_exp\n";
  
  correct = false;
  bool rombeoeCorrect = false, mbeoeCorrect = false;
  Token temp, token1;
  Rest_of_maybe_bit_ex_OR_exp( rombeoeCorrect, token );
  if ( !rombeoeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "|" ) {
    temp = GetToken();
    Maybe_bit_ex_OR_exp( mbeoeCorrect, token1 );
    if ( !mbeoeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    token.value = int( token.value ) | int( token1.value );
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Rest_of_maybe_bit_OR_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Rest_of_maybe_bit_OR_exp()

void Maybe_bit_ex_OR_exp( bool & correct, Token & token ) {
  // maybe_bit_AND_exp { '^' maybe_bit_AND_exp }
  if ( gDEBUGMODE )
    cout << "Maybe_bit_ex_OR_exp\n";
  
  correct = false;
  bool mbaeCorrect = false;
  Token temp, token1;
  Maybe_bit_AND_exp( mbaeCorrect, token );
  if ( !mbaeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "^" ) {
    temp = GetToken();
    Maybe_bit_AND_exp( mbaeCorrect, token1 );
    if ( !mbaeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    token.value = int( token.value ) ^ int( token1.value );
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Maybe_bit_ex_OR_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Maybe_bit_ex_OR_exp()

void Rest_of_maybe_bit_ex_OR_exp( bool & correct, Token & token ) {
  // rest_of_maybe_bit_AND_exp { '^' maybe_bit_AND_exp }
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_bit_ex_OR_exp\n";
  
  correct = false;
  bool rombaeCorrect = false, mbaeCorrect = false;
  Token temp, token1;
  Rest_of_maybe_bit_AND_exp( rombaeCorrect, token );
  if ( !rombaeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "^" ) {
    temp = GetToken();
    Maybe_bit_AND_exp( mbaeCorrect, token1 );
    if ( !mbaeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    token.value = int( token.value ) ^ int( token1.value );
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Rest_of_maybe_bit_ex_OR_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Rest_of_maybe_bit_ex_OR_exp()

void Maybe_bit_AND_exp( bool & correct, Token & token )  {
  // maybe_equality_exp { '&' maybe_equality_exp }
  if ( gDEBUGMODE )
    cout << "Maybe_bit_AND_exp\n";
  
  bool meeCorrect = false;
  Token temp, token1;
  correct = false;
  Maybe_equality_exp( meeCorrect, token );
  if ( !meeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "&" ) {
    temp = GetToken();
    Maybe_equality_exp( meeCorrect, token1 );
    if ( !meeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    token.value = int(token.value) & int(token1.value);
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Maybe_bit_AND_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Maybe_bit_AND_exp()

void Rest_of_maybe_bit_AND_exp( bool & correct, Token & token ) {
  // rest_of_maybe_equality_exp { '&' maybe_equality_exp }
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_bit_AND_exp\n";
  
  bool romeeCorrect = false, meeCorrect = false;
  Token temp, token1;
  correct = false;
  Rest_of_maybe_equality_exp( romeeCorrect, token );
  if ( !romeeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "&" ) {
    temp = GetToken();
    Maybe_equality_exp( meeCorrect, token1 );
    if ( !meeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    token.value = int(token.value) & int(token1.value);
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Rest_of_maybe_bit_AND_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Rest_of_maybe_bit_AND_exp()

void Maybe_equality_exp( bool & correct, Token & token ) {
  // maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp}
  if ( gDEBUGMODE )
    cout << "Maybe_equality_exp\n";
  
  bool mreCorrect = false;
  Token temp, boolean1, token1;
  correct = false;
  Maybe_relational_exp( mreCorrect, token );
  if ( !mreCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "==" || gUnDealToken.str == "!=" ) {
    boolean1 = GetToken();
    Maybe_relational_exp( mreCorrect, token1 );
    if ( !mreCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( boolean1.str == "==" ) {
      if ( fabs( token.value - token1.value ) <= 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // if
    else if ( boolean1.str == "!=" ) {
      if ( fabs( token.value - token1.value ) > 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Maybe_equality_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Maybe_equality_exp()

void Rest_of_maybe_equality_exp( bool & correct, Token & token ) {
  //  rest_of_maybe_relational_exp { ( EQ | NEQ ) maybe_relational_exp }
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_equality_exp\n";
  
  bool mreCorrect = false, romreCorrect = false;
  Token temp, boolean1, token1;
  correct = false;
  Rest_of_maybe_relational_exp( romreCorrect, token );
  if ( !romreCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "==" || gUnDealToken.str == "!=" ) {
    boolean1 = GetToken();
    Maybe_relational_exp( mreCorrect, token1 );
    if ( !mreCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( boolean1.str == "==" ) {
      if ( fabs( token.value - token1.value ) <= 0.0001 ) {
        
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // if
    else if ( boolean1.str == "!=" ) {
      if ( fabs( token.value - token1.value ) > 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Rest_of_maybe_equality_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Rest_of_maybe_equality_exp()

void Maybe_relational_exp( bool & correct, Token & token ) {
  // maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp }
  if ( gDEBUGMODE )
    cout << "Maybe_relational_exp\n";
  
  bool mseCorrect = false;
  Token temp, boolean1, token1;
  correct = false;
  Maybe_shift_exp( mseCorrect, token );
  if ( !mseCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == ">" || gUnDealToken.str == "<"
          || gUnDealToken.str == ">=" || gUnDealToken.str == "<=" ) {
    boolean1 = GetToken();
    Maybe_shift_exp( mseCorrect, token1 );
    if ( !mseCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( boolean1.str == ">" ) {
      if ( ( token.value - token1.value ) > 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // if
    else if ( boolean1.str == "<" ) {
      if ( ( token.value - token1.value ) < -0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if ( boolean1.str == ">=" ) {
      if ( ( token.value - token1.value ) >= -0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if ( boolean1.str == "<=" ) {
      if ( ( token.value - token1.value ) <= 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Maybe_relational_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
  
  correct = true;
} // Maybe_relational_exp()

void Rest_of_maybe_relational_exp( bool & correct, Token & token ) {
  // rest_of_maybe_shift_exp { ( '<' | '>' | LE | GE ) maybe_shift_exp }
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_relational_exp\n";
  
  bool romseCorrect = false, mseCorrect = false;
  Token temp, boolean1, token1;
  correct = false;
  Rest_of_maybe_shift_exp( romseCorrect, token );
  if ( !romseCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == ">" || gUnDealToken.str == "<"
          || gUnDealToken.str == ">=" || gUnDealToken.str == "<=" ) {
    boolean1 = GetToken();
    Maybe_shift_exp( mseCorrect, token1 );
    if ( !mseCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( boolean1.str == ">" ) {
      if ( ( token.value - token1.value ) > 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // if
    else if ( boolean1.str == "<" ) {
      if ( ( token.value - token1.value ) < -0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if ( boolean1.str == ">=" ) {
      if ( ( token.value - token1.value ) >= -0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if ( boolean1.str == "<=" ) {
      if ( ( token.value - token1.value ) <= 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE ) {
    cout << "\nANS in Rest_of_maybe_relational_exp : " ;
    PrintValue( token ) ;
    cout << "\n";
  } // if
    
  correct = true;
} // Rest_of_maybe_relational_exp()

void Maybe_shift_exp( bool & correct, Token & token ) {
  //  maybe_additive_exp { ( LS | RS ) maybe_additive_exp }
  if ( gDEBUGMODE )
    cout << "Maybe_shift_exp\n";
  
  bool maeCorrect = false;
  Token temp, token1;
  correct = false;
  Maybe_additive_exp( maeCorrect, token );
  if ( !maeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  PeekToken();
  while ( gUnDealToken.str == "<<" || gUnDealToken.str == ">>" ) {
    temp = GetToken();
    Maybe_additive_exp( maeCorrect, token1 );
    if ( !maeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( temp.str == "<<" ) {
      token.value = ( int ) token.value << ( int ) token1.value;
    } // if
    else {
      token.value = ( int ) token.value >> ( int ) token1.value;
    } // else
      
    PeekToken();
  } // while
  
  if ( gDEBUGMODE )
    cout << "\nANS in Maybe_shift_exp : " << token.value << "\n";
  
  correct = true;
} // Maybe_shift_exp()

void Rest_of_maybe_shift_exp( bool & correct, Token & token ) {
  // rest_of_maybe_additive_exp { ( LS | RS ) maybe_additive_exp }
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_shift_exp\n";
  
  bool romaeCorrect = false, maeCorrect = false;
  Token temp, token1;
  correct = false;
  Rest_of_maybe_additive_exp( romaeCorrect, token );
  if ( !romaeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "<<" || gUnDealToken.str == ">>" ) {
    temp = GetToken();
    Maybe_additive_exp( maeCorrect, token1 );
    if ( !maeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( temp.str == "<<" ) {
      token.value = ( int ) token.value << ( int ) token1.value;
    } // if
    else {
      token.value = ( int ) token.value >> ( int ) token1.value;
    } // else
      
    PeekToken();
  } // while
  
  if ( gDEBUGMODE )
    cout << "\nANS in Rest_of_maybe_shift_exp : " << token.value << "\n";
  correct = true;
} // Rest_of_maybe_shift_exp()

void Maybe_additive_exp(  bool & correct, Token & token  ) {
  //  maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }
  if ( gDEBUGMODE )
    cout << "Maybe_additive_exp\n";
  
  bool mmeCorrect = false;
  Token temp, token1;
  correct = false;
  Maybe_mult_exp( mmeCorrect, token );
  if ( !mmeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "+" || gUnDealToken.str == "-" ) {
    temp = GetToken();
    Maybe_mult_exp( mmeCorrect, token1 );
    if ( !mmeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( token1.value_type == FLOAT ) {
      token.value_type = FLOAT;
      if ( temp.str == "+" ) {
        token.value += token1.value;
      } // if
      else {
        token.value -= token1.value;
      } // else
      
    } // if
    else if ( token1.value_type == INT ) {
      if ( temp.str == "+" ) {
        token.value += token1.value;
      } // if
      else {
        token.value -= token1.value;
      } // else
      
    } // else if
    else if ( token1.value_type == STRING ) {
     
    } // else if
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE )
    cout << "\nANS in Maybe_additive_exp : " << token.value << "\n";
  correct = true;
} // Maybe_additive_exp()

void Rest_of_maybe_additive_exp( bool & correct, Token & token ) {
  // rest_of_maybe_mult_exp { ( '+' | '-' ) maybe_mult_exp }
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_additive_exp\n";
  
  bool rommeCorrect = false, mmeCorrect = false;
  Token temp, token1;
  correct = false;
  Rest_of_maybe_mult_exp( rommeCorrect, token );
  if ( !rommeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while ( gUnDealToken.str == "+" || gUnDealToken.str == "-" ) {
    temp = GetToken();
    Maybe_mult_exp( mmeCorrect, token1 );
    if ( !mmeCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( token1.value_type == FLOAT ) {
      token.value_type = FLOAT;
      if ( temp.str == "+" ) {
        token.value += token1.value;
      } // if
      else {
        token.value -= token1.value;
      } // else
      
    } // if
    else if ( token1.value_type == INT ) {
      if ( temp.str == "+" ) {
        token.value += token1.value;
      } // if
      else {
        token.value -= token1.value;
      } // else
      
    } // else if
    else if ( token1.value_type == STRING ) {
     
    } // else if
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE )
    cout << "\nANS in Rest_of_maybe_additive_exp : " << token.value << "\n";
  correct = true;
} // Rest_of_maybe_additive_exp()

void Maybe_mult_exp( bool & correct, Token & token ) {
  // unary_exp rest_of_maybe_mult_exp
  if ( gDEBUGMODE )
    cout << "Maybe_mult_exp\n";
  
  bool ueCorrect = false, rommeCorrect = false;
  correct = false;
  Unary_exp( ueCorrect, token );
  if ( !ueCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  Rest_of_maybe_mult_exp( rommeCorrect, token );
  if ( !rommeCorrect ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  correct = true;
} // Maybe_mult_exp()

void Rest_of_maybe_mult_exp( bool & correct, Token & token ) {
  // { ( '*' | '/' | '%' ) unary_exp } /* could be empty ! */
  if ( gDEBUGMODE )
    cout << "Rest_of_maybe_mult_exp\n";
  
  bool ueCorrect = false;
  Token temp, token1;
  correct = false;
  PeekToken();
  while ( gUnDealToken.str == "*" || gUnDealToken.str == "/" || gUnDealToken.str == "%" ) {
    temp = GetToken();
    Unary_exp( ueCorrect, token1 );
    if ( !ueCorrect ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    if ( token1.value_type == FLOAT ) {
      token.value_type = FLOAT;
    } // if
    
    if ( temp.str == "*" ) {
      token.value = token.value * token1.value;      
    } // if
    else if ( temp.str == "/" ) {
      token.value = token.value / token1.value;
      if ( token.value_type == INT && token.value >= 0 ) {
        token.value = floor( token.value );
      } // if
      else if ( token.value_type == INT && token.value < 0 ) {
        token.value = ceil( token.value );
      } // else if
      
    } // else if
    else if ( temp.str == "%" ) {
      token.value = ( int ) token.value % ( int ) token1.value;
    } // else if
    
    PeekToken();
  } // while
  
  if ( gDEBUGMODE )
    cout << "\nANS in Rest_of_maybe_mult_exp: " << token.value << "\n";
  correct = true;
} // Rest_of_maybe_mult_exp()

void Unary_exp( bool & correct, Token & token ) {
  // sign { sign } signed_unary_exp | unsigned_unary_exp | ( PP | MM ) Identifier [ '[' expression ']' ]
  if ( gDEBUGMODE )
    cout << "Unary_exp\n";
  
  bool sCorrect = false, uueCorrect = false, sueCorrect = false;
  bool eCorrect = false, defined = false;
  int signCount1 = 0, signCount2 = 0, signCount3 = 0;
  Token temp, sign;
  correct = false;
  Sign( sCorrect, sign );
  if ( sCorrect ) {
    if ( sign.str == "!" ) {
      signCount3++;
    } // if
    else if ( sign.str == "-" ) {
      signCount2++;
    } // else if
    else {
      signCount1++;
    } // else
    
    
    while ( sCorrect ) {
      Sign( sCorrect, sign );
      if ( sign.str == "!" ) {
        signCount3++;
      } // if
      else if ( sign.str == "-" ) {
        signCount2++;
      } // else if
      else if ( sign.str == "+" ) {
        signCount1++;
      } // else if
      else {
       
      } // else
      
    } // while
    
    Signed_unary_exp( sueCorrect, token );     
    if ( !sueCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    if ( signCount3 != 0 ) {
      if ( signCount3 % 2 == 0 ) {
        if ( token.value != 0 ) {
          token.value = 0;
        } // if
        else {
          token.value = 1;
        } // else
        
      } // if
      else {
        if ( token.value != 0 ) {
          token.value = 1;
        } // if
        else {
          token.value = 0;
        } // else
        
      } // else
      
    } // if
    
    if ( signCount2 % 2 != 0 ) {
      token.value = 0 - token.value;
    } // if
    
    correct = true;
  } // if
  else {
    Unsigned_unary_exp( uueCorrect, token );
    if ( !uueCorrect ) {
      PeekToken();
      if ( gUnDealToken.str == "++" || gUnDealToken.str == "--" ) {
        temp = GetToken();
        token = GetToken();
        if ( token.type != IDENT ) {
          ErrorHandler( token.str, 2 );
        } // if
        
        // 在此檢查是否define過
        // if gInFunction ==> 找Local和Global
        // else 找 Global 和 Function
        if ( gCompoundTime > 0 ) {
          for ( int i = 0; i < gLocalVariable.size() ; i++ ) {
            if ( token.str == gLocalVariable.at( i ).name ) {
              if ( !gLocalVariable.at( i ).isArray ) {
                token = gLocalVariable.at( i ).token;
                if ( temp.str == "++" ) {
                  token.value += 1;
                  gLocalVariable.at( i ).token.value += 1;
                } // if
                else {
                  token.value -= 1;
                  gLocalVariable.at( i ).token.value -= 1;
                } // else
                
              } // if
              
              defined = true;
            } // if
          } // for
          
          if ( !defined ) {
            for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
              if ( token.str == gGlobalVariable.at( i ).name ) {
                if ( !gGlobalVariable.at( i ).isArray ) {
                  token = gGlobalVariable.at( i ).token;
                  if ( temp.str == "++" ) {
                    token.value += 1;
                    gGlobalVariable.at( i ).token.value += 1;
                  } // if
                  else {
                    token.value -= 1;
                    gGlobalVariable.at( i ).token.value -= 1;
                  } // else
                  
                } // if
                
                defined = true;
              } // if
            } // for
          } // if
                    
          if ( !defined ) {
            ErrorHandler( token.str, 3 );
          } // if
          
        } // if
        else {
          for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
            if ( token.str == gGlobalVariable.at( i ).name ) {
              if ( !gGlobalVariable.at( i ).isArray ) {
                token = gGlobalVariable.at( i ).token;
                if ( temp.str == "++" ) {
                  token.value += 1;
                  gGlobalVariable.at( i ).token.value += 1;
                } // if
                else {
                  token.value -= 1;
                  gGlobalVariable.at( i ).token.value -= 1;
                } // else
                
              } // if
              
              defined = true;
            } // if
          } // for
          
          if ( !defined ) {
            for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
              if ( token.str == gFunctionTable.at( i ).name ) {
                defined = true;
              } // if
            } // for
          } // if
          
          if ( !defined ) {
            ErrorHandler( token.str, 3 );
          } // if
          
        } // else 
        
        PeekToken();
        if ( gUnDealToken.str == "[" ) {
          Expression( eCorrect, token );
          if ( !eCorrect ) {
            ErrorHandler( token.str, 2 );
          } // if
          
          temp = GetToken();
          if ( temp.str != "]" ) {
            ErrorHandler( temp.str, 2 );
          } // if
          
        } // if
        
      } // if
     
    } // if
    
    correct = true;
    
  } // else
    
} // Unary_exp()

void Signed_unary_exp( bool & correct, Token & token ) {
  // Identifier [ '(' [ actual_parameter_list ] ')' | '[' expression ']'] 
  // | Constant | '(' expression ')'
  if ( gDEBUGMODE )
    cout << "Signed_unary_exp\n";
   
  Token temp;
  bool aplCorrect = false, eCorrect = false;
  bool defined = false;
  correct = false;
  PeekToken();
  if ( gUnDealToken.type == IDENT ) {
    // 此處取得第2+個IDENT
    token = GetToken();
    // if gInFunction ==> 找Local和Global
    // else 找 Global 和 Function
    if ( gCompoundTime > 0 ) {
      for ( int i = 0; i < gLocalVariable.size() ; i++ ) {
        if ( token.str == gLocalVariable.at( i ).name ) {
          if ( !gLocalVariable.at( i ).isArray ) {
            token = gLocalVariable.at( i ).token;
          } // if
          
          defined = true;
        } // if
      } // for
      
      if ( !defined ) {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( token.str == gGlobalVariable.at( i ).name ) {
            if ( !gGlobalVariable.at( i ).isArray ) {
              token = gGlobalVariable.at( i ).token;
            } // if
            
            defined = true;
          } // if
        } // for
      } // if
      
      if ( !defined ) {
        for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
          if ( token.str == gFunctionTable.at( i ).name ) {
            defined = true;
          } // if
        } // for
      } // if
      
      if ( !defined ) {
        ErrorHandler( token.str, 3 );
      } // if
      
    } // if
    else {
      for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
        if ( token.str == gGlobalVariable.at( i ).name ) {
          if ( !gGlobalVariable.at( i ).isArray ) {
            token = gGlobalVariable.at( i ).token;
          } // if
          
          defined = true;
        } // if
      } // for
      
      if ( !defined ) {
        for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
          if ( token.str == gFunctionTable.at( i ).name ) {
            defined = true;
          } // if
        } // for
      } // if
      
      if ( !defined ) {
        ErrorHandler( token.str, 3 );
      } // if
      
    } // else 
    
    PeekToken();
    if ( gUnDealToken.str == "(" ) {
      Actual_parameter_list( aplCorrect, token );
      PeekToken();
      if ( gUnDealToken.str != ")" ) {
        temp = GetToken();
        ErrorHandler( temp.str, 2 );
      } // if
      
      correct = true;
    } // if
    else if ( gUnDealToken.str == "[" ) {
      temp = GetToken();
      Expression( eCorrect, token );
      if ( !eCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      PeekToken();
      if ( gUnDealToken.str != "]" ) {
        temp = GetToken();
        ErrorHandler( temp.str, 2 );
      } // if
      
      correct = true;
    } // else if
    
    correct = true;
  } // if
  else if ( gUnDealToken.type == CONST ) {
    token = GetToken();
    if ( token.value_type == INT ) {
      token.value = atoi( token.str.c_str() );
    } // if
    else if ( token.value_type == INT ) {
      token.value = atof( token.str.c_str() );
    } // else if
    
    correct = true;
  } // else if
  else if ( gUnDealToken.str == "(" ) {
    temp = GetToken();
    Expression( eCorrect, token );
    if ( !eCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    PeekToken();
    if ( gUnDealToken.str != ")" ) {
      temp = GetToken();
      ErrorHandler( temp.str, 2 );
    } // if
    
    correct = true;
  } // else if

} // Signed_unary_exp()

void Unsigned_unary_exp( bool & correct, Token & token ) {
  // Identifier [ '(' [ actual_parameter_list ] ')' | [ '[' expression ']' ] [ ( PP | MM ) ] ]
  // | Constant | '(' expression ')'
  if ( gDEBUGMODE )
    cout << "Unsigned_unary_exp\n";
  
  Token temp;
  bool aplCorrect = false, eCorrect = false;
  bool defined = false;
  correct = false;
  PeekToken();
  if ( gUnDealToken.type == IDENT ) {
    // 此處取得第2+個IDENT
    token = GetToken();
    // else 找 Global 和 Function
    if ( gCompoundTime > 0 ) {
      for ( int i = 0; i < gLocalVariable.size() ; i++ ) {
        if ( token.str == gLocalVariable.at( i ).name ) {
          if ( !gLocalVariable.at( i ).isArray ) {
            token = gLocalVariable.at( i ).token;
          } // if
          
          defined = true;
        } // if
      } // for
      
      if ( !defined ) {
        for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
          if ( token.str == gGlobalVariable.at( i ).name ) {
            if ( !gGlobalVariable.at( i ).isArray ) {
              token = gGlobalVariable.at( i ).token;
            } // if
            
            defined = true;
          } // if
        } // for
      } // if
      
      if ( !defined ) {
        for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
          if ( token.str == gFunctionTable.at( i ).name ) {
            defined = true;
          } // if
        } // for
      } // if
      
      if ( !defined ) {
        ErrorHandler( token.str, 3 );
      } // if
      
    } // if
    else {
      for ( int i = 0; i < gGlobalVariable.size() ; i++ ) {
        if ( token.str == gGlobalVariable.at( i ).name ) {
          if ( !gGlobalVariable.at( i ).isArray ) {
            token = gGlobalVariable.at( i ).token;
          } // if
          
          defined = true;
        } // if
      } // for
      
      if ( !defined ) {
        for ( int i = 0; i < gFunctionTable.size() ; i++ ) {
          if ( token.str == gFunctionTable.at( i ).name ) {
            defined = true;
          } // if
        } // for
      } // if
      
      if ( !defined ) {
        ErrorHandler( token.str, 3 );
      } // if
      
    } // else 
    
    PeekToken();
    if ( gUnDealToken.str == "(" ) {
      temp = GetToken();
      Actual_parameter_list( aplCorrect, token );
      PeekToken();
      if ( gUnDealToken.str != ")" ) {
        temp = GetToken();
        ErrorHandler( temp.str, 2 );
      } // if
      
      
      temp = GetToken();
      correct = true;
    } // if
    else if ( gUnDealToken.str == "[" ) {
      temp = GetToken();
      Expression( eCorrect, token );
      if ( !eCorrect ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      PeekToken();
      if ( gUnDealToken.str != "]" ) {
        temp = GetToken();
        ErrorHandler( temp.str, 2 );
      } // if
      
      temp = GetToken();
      PeekToken();
      if ( gUnDealToken.str == "++" || gUnDealToken.str == "--" ) {
        temp = GetToken();
      } // else if

    } // else if
    else if ( gUnDealToken.str == "++" || gUnDealToken.str == "--" ) {
      temp = GetToken();
    } // else if
    
    correct = true;
  } // if
  else if ( gUnDealToken.type == CONST ) {
    token = GetToken();
    if ( token.value_type == INT ) {
      token.value = atoi( token.str.c_str() );
    } // if
    else if ( token.value_type == INT ) {
      token.value = atof( token.str.c_str() );
    } // else if
    
    correct = true;
  } // else if
  else if ( gUnDealToken.str == "(" ) {
    temp = GetToken();
    Expression( eCorrect, token );
    if ( !eCorrect ) {
      ErrorHandler( token.str, 2 );
    } // if
    
    PeekToken();
    if ( gUnDealToken.str != ")" ) {
      temp = GetToken();
      ErrorHandler( temp.str, 2 );
    } // if
    
    temp = GetToken();
    correct = true;
  } // else if
  else {
    temp = GetToken();
    ErrorHandler( temp.str, 2 );
  } // else

} // Unsigned_unary_exp()


int main() {
  Token token;
  bool programContinue = true;
  int testNumber;
  char enter;
  Initialize( gUnDealToken );
  Initialize( token );
  cout << "Our-C running ...\n";
  cin >> testNumber;
  enter = cin.get();
  while ( programContinue == true ) {
    try {
      User_input( programContinue );
    } // try 
    catch( int err ) {
      
    } // catch()
    
  } // while

  cout << "> Our-C exited ...\n";
} // main()

