#include<iostream>
#include<stdio.h>
#include<string>
#include<vector>
#include <stdlib.h>
#include <cstdlib>
#include<iomanip>
#include<exception>
#include<cmath>

using namespace std;

int gLine = 1, gColumn = 0;
char gUnDealCharacter = '\0';
bool gIsEOF = false;

enum Type{
  END_OF_FILE, NONE, IDENT, NUM, OPERATOR, INT, FLOAT, CHAR, BOOL, STRING, VOID, IF, ELSE, WHILE, DO, RETURN, GE, LE, EQ, ADD, SUB, THAN, LESS,
  NEQ, AND, OR, PE, ME, TE, DE, RE, PP, MM, RS, LS, LP, RP, LMP, RMP, LBP, RBP, XOR, SEM, COM, QUE, COL, MULT, DIV, ASSIGN, PER, LAND,
  LOR, EXC
};

struct Token {
  Type type ;
  string str;
  Type value_type;
  double value;
};

vector<Token> gIDTable;
int gERRORCODE = 0;
Token gUnDealToken;
bool gPROGRAMEND = false;
bool gDEBUGMODE = false;

void Command( bool & correct );
void IDlessArithExpOrBexp( bool & correct, Token & token );
void BooleanOperator( bool & correct, Token & token );
void Not_ID_StartArithExpOrBexp( bool & correct, Token & token );
void Not_ID_StartArithExp( bool & correct, Token & token );
void Not_ID_StartTerm( bool & correct, Token & token );
void Not_ID_StartFactor( bool & correct, Token & token );
void ArithExp( bool & correct, Token & token );
void Term( bool & correct, Token & token );
void Factor( bool & correct, Token & token );
void ErrorHandler( string str , int errorType );
void PrintValue( Token token );

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

void PrintValue( Token token ) {
  if( token.value_type == BOOL && token.value == 1 ) {
				cout << "> true\n";
	 } //if
		else if( token.value_type == BOOL && token.value == 0 ) {
				cout << "> false\n";
		} // else if
		else if( token.value_type == FLOAT ) {
				//cout << value << "\n";
				printf("> %.3f\n", token.value);
		} // else if
		else if( token.value_type == INT ) {
				cout << "> " << token.value << "\n";
	 } // else if
	 
} // PrintValue()


char Get_the_next_char() {
  // to read a character 
  // and keep track of the line number and gcolumn number of this character
  // return this character
  char ch = '\0';
  ch = cin.get();
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
      while ( ch != '\n' ) { //read until '\n'.
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
  //F3 
  // An IDENT starts with a letter and followed by digits or underglines
  Token token;
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
  return token;

} // Get_all_ident()

Token Get_all_constant( char firstCharacter ) {
  //F5
  // A num can be a integer or a float
  // e.g. 35, 7, 43.8, 43., .35, 1.0, 07
 	
  Token token;
  char ch = '\0';
  bool isFloat = false;
  string tokenStr = "";
  Initialize( token );
  token.type = NUM;
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
  while( ( ch >= '0' && ch <= '9' ) || ch == '.' ) {
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
  //F4
  // A SIGN can be '+' or '-' or '*' or '/'.....
  Token token;
  char ch = '\0';
  string tokenStr = "";
  Initialize( token );
  
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
  else if ( tokenStr == "+" ) {
    token.type = ADD;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "-" ) {
    token.type = SUB;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "*" ) {
    token.type = MULT;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "/" ) {
    token.type = DIV;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == "=" ) {
    token.type = EQ;
    token.str = tokenStr;
    return token;
  } // else if
  else if ( tokenStr == ";" ) {
    token.type = SEM;
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
    else if( ch == '>' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = NEQ;
      token.str = tokenStr;
      return token;
    } // else if
    else {
      token.type = LESS;
      token.str = tokenStr;
      return token;
    } // else
    
  } // else if
  /*
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
      //cout << "Unrecognized with first char : '" << tokenStr << "'\n";
      ErrorHandler(tokenStr, 1);
      // Unrecognized token : '!' exist
    } // else
  } // else if
  */
  else if ( tokenStr == ":" ) {
    ch = cin.peek();
    if ( ch == '=' ) {
      ch = Get_the_next_char();
      tokenStr += ch;
      token.type = ASSIGN;
      token.str = tokenStr;
      return token;
    } // if
    else {
      ErrorHandler(tokenStr, 1);
    } // else
    
  } // else if
  else {
    ErrorHandler(tokenStr, 1);
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
    Initialize(gUnDealToken);
  } // if
  else {
    // 沒有尚未處理的token, 從input取資料 
    ch = Get_the_next_non_white_space_char();
    if ( ch == EOF ) {
      token.type = END_OF_FILE;
    } // if
    else if ( ( ch >= 'A' && ch <= 'Z' ) || ( ch >= 'a' && ch <= 'z' ) ) {
      token = Get_all_ident(ch);
    } // else if
    else if ( ( ch >= '0' && ch <= '9' ) || ch == '.' ) {
      token = Get_all_constant( ch );
    } // else if
    else {
      // the ERROR: Unrecognized token will occur in Get_all_operator() function.
      token = Get_all_operator( ch );
    } // else
  } // else

  return token;
} // GetToken()

void PeekToken() {
  gUnDealToken = GetToken();
} // PeekToken()

void ErrorHandler( string str, int errorType ) {
  char ch = '\0';
  if( errorType == 0 ) {
    cout << "> Error\n" ;
    // do nothing
  } // if
  else if( errorType == 1 ) {
    cout << "> Unrecognized token with first char : '" + str +  "'\n";
  } // else if
  else if( errorType == 2 ) {
    cout << "> Unexpected token : '" + str +  "'\n";
  } // else if
  else { 
   cout << "> Undefined identifier : '" + str +  "'\n";
  } // else
  
  while( ch != '\n' ) {
    ch = Get_the_next_char();
  } // while
  
  Initialize(gUnDealToken);
  throw errorType;
} // ErrorHandler()

void Command( bool & correct ) {
  // <Command> ::= IDENT ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';'
  //               | <Not_ID_StartArithExpOrBexp> ';'
  //               | QUIT	
  if( gDEBUGMODE ) {
    cout << "Command\n";
  } // if
  
  Token temp, token, token1;
  Initialize(temp);
  Initialize(token);
  Initialize(token1);
  bool defined = false;
  bool correct1 = true;
  correct = true;
  
  try{
    
    PeekToken();
    if( gUnDealToken.str == "quit" ) {
      token = GetToken();
      correct = false;
      return;
    } // if
    else if( gUnDealToken.type == IDENT ) {
      //IDENT ( ':=' <ArithExp> | <IDlessArithExpOrBexp> ) ';'
      token = GetToken(); // IDENT
      
      for( int i = 0; i < gIDTable.size(); i++ ) {
        if( token.str == gIDTable.at(i).str ) {
          token = gIDTable.at(i);
          defined = true;
        } // if
      } // for
      
      PeekToken();
      if( gUnDealToken.str == ":=" ) {
        // <ArithExp>
        temp = GetToken(); // ':='
        ArithExp( correct, token1 );
        if( !correct ) {
          ErrorHandler( token1.str, 2 ); 
        } // if
        
        PeekToken();
        if( gUnDealToken.str == ";" ) {
          // 有分號 正確!!
          temp = GetToken(); // ';'
          token.value = token1.value;
          token.value_type = token1.value_type;
          
          if( !defined ) {
            // 未被定義過就在此定義  
            gIDTable.push_back(token);
          } // if
          else {
            // 已被定義過就在此更改原本的值 
            for( int i = 0; i < gIDTable.size(); i++ ) {
              if( token.str == gIDTable.at(i).str ) {
                gIDTable.at(i) = token;
              } // if
              
            } // for
            
          } // else
          
          // 印出他的值
          PrintValue( token );
        } // if
        else {
          // 無分號 ERROR
          temp = GetToken(); // ??
          ErrorHandler( temp.str, 2 );
        } // else
        
      } // if
      else {
        // <IDlessArithExpOrBexp>
        IDlessArithExpOrBexp( correct1, token );
        /*
        if( correct1 ) {
          // IDlessArithExpOrBexp 有東西就要看有沒有被define過
          if( !defined ) {
            ErrorHandler( token.str, 3 );
          } // if
          
          PeekToken();
          if( gUnDealToken.str != ";" ) {
            temp = GetToken();
            ErrorHandler( temp.str, 2 );
          } // if
          // 回存他的值
          for( int i = 0; i < gIDTable.size(); i++ ) {
            if( token.str == gIDTable.at(i).str ) {
              gIDTable.at(i) = token;
            } // if

          } // for
          // 印出他的值
          temp = GetToken();
          PrintValue( token );
          
        } // if
        else {
          PeekToken();
          if( gUnDealToken.str != ";" ) {
            temp = GetToken();
            ErrorHandler( temp.str, 2 );
          } // if
          
          if( !defined ) {
            ErrorHandler( token.str, 3 );
          } // if
          //印出他的值
          temp = GetToken();
          PrintValue( token );
        } // else
        */
        PeekToken();
        if( gUnDealToken.str != ";" ) {
          temp = GetToken();
          ErrorHandler( temp.str, 2 );
        } // if
        
        if( !defined ) {
          ErrorHandler( token.str, 3 );
        } // if
        /*
        // 回存他的值
        for( int i = 0; i < gIDTable.size(); i++ ) {
          if( token.str == gIDTable.at(i).str ) {
            gIDTable.at(i) = token;
          } // if

        } // for
        */
        //印出他的值
        temp = GetToken();
        PrintValue( token );
      } // else

    } // else if
    else {
      // <Not_ID_StartArithExpOrBexp> ';'
      Not_ID_StartArithExpOrBexp( correct, token );
      
      if( !correct ) {
        ErrorHandler( token.str, 2 );
      } // if
      
      PeekToken();
      if( gUnDealToken.str != ";" ) {
        temp = GetToken();
        ErrorHandler( temp.str, 2 );
      } // if
      
      temp = GetToken();
      PrintValue( token );
    } // else
    
  } // try
  catch( int err ){
    return;
  } // catch
} // Command()

void IDlessArithExpOrBexp( bool & correct, Token & token ) {
  if( gDEBUGMODE ) {
    cout << "IDlessArithExpOrBexp\n";
  } // if
  // <IDlessArithExpOrBexp> ::= { '+' <Term>   | '-' <Term>
  //                              '*' <Factor> | '/' <Factor> 
  //                            }
  //                            [ <BooleanOperator> <ArithExp> ]
  
  Token temp, token1, boolean;
  bool correct1 = false;
  bool defined = false;
  Initialize(temp);
  Initialize(token1);
  Initialize(boolean);
  correct = false; 
  
  for( int i = 0; i < gIDTable.size(); i++ ) {
    if( token.str == gIDTable.at(i).str ) {
      token = gIDTable.at(i);
      defined = true;
    } // if
  } // for
  
  PeekToken();
  while( gUnDealToken.str == "+" || gUnDealToken.str == "-" 
         || gUnDealToken.str == "*" || gUnDealToken.str == "/" ) {
    temp = GetToken(); // '+' || '-' || '*' || '/'
    
    if( !defined ) {
      ErrorHandler( token.str, 3 );
    } // if
    
    if( temp.str == "+" ) {
      Term( correct1, token1 );
      if( !correct1 ) {
        // 有 '+' 或 '-' 一定要有Term
        ErrorHandler( token1.str, 2 );
      } // if

      token.value = token.value + token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
      
    } // if
    else if( temp.str == "-" ) {
      Term( correct1, token1 );
      if( !correct1 ) {
        // 有 '+' 或 '-' 一定要有Term
        ErrorHandler( token1.str, 2 );
      } // if
      
      token.value = token.value - token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
     
    } // else if
    else if( temp.str == "*" ) {
      Factor( correct1, token1 );
      if( !correct1 ) {
        // 有 '*' 或 '/' 一定要有Factor
        ErrorHandler( token1.str, 2 );
      } // if
      
      token.value = token.value * token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
     
    } // else if
    else if( temp.str == "/" ) {
      Factor( correct1, token1 );
      if( !correct1 ) {
        // 有 '*' 或 '/' 一定要有Factor
        ErrorHandler( token1.str, 2 );
      } // if
      
      if( token1.value == 0 ) {
        ErrorHandler("", 0 );
      } // if
      
      token.value = token.value / token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
      
      if ( token.value_type == INT && token.value >= 0 ) {
        token.value = floor( token.value );
      } // if
      else if( token.value_type == INT && token.value < 0 ) {
        token.value = ceil( token.value );
      } // else
     
    } // else if
    
    correct = true;
    PeekToken();
  } // while
  
  // [ <BooleanOperator> <ArithExp> ]
  // 可有可無 
  BooleanOperator( correct1, boolean );
  if( !correct1 ) {
    // correct = true;(有進去while correct就會是true)
    return;
  } // if
  else {
    // 有<BooleanOperator>一定要有 <ArithExp>
    if( !defined ) {
      ErrorHandler( token.str, 3 );
    } // if
    
    ArithExp( correct, token1 );
    if( !correct ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    // 比token和token1的大小
    if( boolean.str == "=" ) {
      if( fabs( token.value - token1.value ) <= 0.0001 ) {
        
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // if
    else if( boolean.str == "<>" ) {
      if( fabs( token.value - token1.value ) > 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if( boolean.str == ">" ) {
      if( (token.value - token1.value) > 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if( boolean.str == "<" ) {
      if( (token.value - token1.value) < -0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if( boolean.str == ">=" ) {
      if( (token.value - token1.value) >= -0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if( boolean.str == "<=" ) {
      if( (token.value - token1.value) <= 0.0001 ) {
         token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    
    correct = true;
  } // else
  
} // IDlessArithExpOrBexp()

void BooleanOperator( bool & correct, Token & token ) {
  // <BooleanOperator> ::= '=' | '<>' | '>' | '<' | '>=' | '<='
  if( gDEBUGMODE ) {
    cout << "BooleanOperator\n";
  } // if
  
  PeekToken();
  if( gUnDealToken.str != "=" && gUnDealToken.str != "<>" && gUnDealToken.str != ">" 
      && gUnDealToken.str != "<"  && gUnDealToken.str != ">=" && gUnDealToken.str != "<=" ) {
    correct = false;
    //ErrorHandler( token.str, 2 );
  } // if
  else {
    token = GetToken();
    correct = true;
  } // else

} // BooleanOperator()

void Not_ID_StartArithExpOrBexp( bool & correct, Token & token ) {
  // <Not_ID_StartArithExpOrBexp> ::= <Not_ID_StartArithExp>
  //                                  [ <BooleanOperator> <ArithExp> ]
  if( gDEBUGMODE ) {
    cout << "Not_ID_StartArithExpOrBexp\n";
  } // if

  Token temp, token1, boolean;
  bool correct1 = false;
  Initialize(temp);
  Initialize(token1);
  Initialize(boolean);
  
  Not_ID_StartArithExp( correct, token );
  if( !correct ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  // [ <BooleanOperator> <ArithExp> ]
  // 可有可無 
  BooleanOperator( correct1, boolean );
  if( !correct1 ) {
    // correct = true;
    return;
  } // if
  else {
    // 有<BooleanOperator>一定要有 <ArithExp>
    ArithExp( correct, token1 );
    if( !correct ) {
      ErrorHandler( token1.str, 2 );
    } // if
    
    // 比token和token1的大小
    if( boolean.str == "=" ) {
      if( fabs( token.value - token1.value ) <= 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // if
    else if( boolean.str == "<>" ) {
      if( fabs( token.value - token1.value ) > 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if( boolean.str == ">" ) {
      if( (token.value - token1.value) > 0.0001 ) {
        token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if( boolean.str == "<" ) {
      if( (token.value - token1.value) < -0.0001 ) {
         token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if( boolean.str == ">=" ) {
      if( (token.value - token1.value) >= -0.0001 ) {
         token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    else if( boolean.str == "<=" ) {
      if( (token.value - token1.value) <= 0.0001 ) {
         token.value = 1;
      } // if
      else {
        token.value = 0;
      } // else
      
      token.value_type = BOOL;
    } // else if
    
  } // else 


} // Not_ID_StartArithExpOrBexp()

void Not_ID_StartArithExp( bool & correct, Token & token ) {
  // <Not_ID_StartArithExp> ::= <Not_ID_StartTerm> { '+' <Term> | '-' <Term> }
  if( gDEBUGMODE ) {
    cout << "Not_ID_StartArithExp\n";
  } // if
  
  Token temp, token1;
  Initialize(temp);
  Initialize(token1);
  
  Not_ID_StartTerm( correct, token );
  if( !correct ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  
  while( gUnDealToken.str == "+" || gUnDealToken.str == "-" ) {
    temp = GetToken(); // '+' || '-'
    Term( correct, token1 );
    if( !correct ) {
      // 有 '+' 或 '-' 一定要有Term
      ErrorHandler( token1.str, 2 );
    } // if

    if( temp.str == "+" ) {
      token.value = token.value + token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
      
    } // if
    else {
      token.value = token.value - token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
     
    } // else
    
    PeekToken();
  } // while

} // Not_ID_StartArithExp()

void Not_ID_StartTerm( bool & correct, Token & token ) {
  // <Not_ID_StartTerm> ::= <Not_ID_StartFactor> { '*' <Factor> | '/' <Factor> }
  if( gDEBUGMODE ) {
    cout << "Not_ID_StartTerm\n";
  } // if
  
  Token temp, token1;
  Initialize(temp);
  Initialize(token1);
  
  Not_ID_StartFactor( correct, token );
  if( !correct ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while( gUnDealToken.str == "*" || gUnDealToken.str == "/" ) {
    temp = GetToken(); // '*' || '/'
    Factor( correct, token1 );
    if( !correct ) {
      // 有 '*' 或 '/' 一定要有Factor 
      ErrorHandler( token1.str, 2 );
    } // if
    
    if( temp.str == "*" ) {
      token.value = token.value * token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
      
    } // if
    else {
      if ( token1.value == 0 ) {
        ErrorHandler("", 0 );
      } // if
      
      token.value = token.value / token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
      
      if ( token.value_type == INT && token.value >= 0 ) {
        token.value = floor( token.value );
      } // if
      else if ( token.value_type == INT && token.value < 0 ) {
        token.value = ceil( token.value );
      } // else
     
    } // else
    
    PeekToken();
  } // while

} // Not_ID_StartTerm()

void Not_ID_StartFactor( bool & correct, Token & token ) {
  // <Not_ID_StartFactor> ::= [SIGN] NUM | '(' <ArithExp> ')'
  if( gDEBUGMODE ) {
    cout << "Not_ID_StartFactor\n";
  } // if
  
  Token temp, token1;
  Initialize(temp);
  Initialize(token1);
  
  PeekToken();
  if( gUnDealToken.str == "+" ) {
    // 遇到sign，則一定要有NUM 
    temp = GetToken(); // '+'
    token = GetToken();
    if( token.type != NUM ) {
      // 發生unexpected，讀掉剩餘部分
      //correct = false;
      ErrorHandler( token.str, 2 );
      //return;
    } // if
    else {
      //正確無誤
      correct = true;
      return;
    } // else

  } // if
  else if( gUnDealToken.str == "-" ) {
    // 遇到sign，則一定要有NUM 
    temp = GetToken(); // '-'
    token = GetToken();
    
    if( token.type != NUM ) {
      // 發生unexpected，讀掉剩餘部分 
      //correct = false;
      ErrorHandler( token.str, 2 );
      //return;
    } // if
    else {
      token.value = 0 - token.value;
      correct = true;
      return;
    } // else
    
  } // else if
  else if( gUnDealToken.type == NUM ) {
    token = GetToken();
    correct = true;
    return;
  } // else if
  else if( gUnDealToken.str == "(" ) {
    // '(' <ArithExp> ')' 
    temp = GetToken(); // '('
    ArithExp( correct, token );
    if( correct ) {
      temp = GetToken(); // ')'
      if( temp.str != ")" ) {
        //correct = false; 
        ErrorHandler( temp.str, 2 );
      } // if
      else {
        correct = true;
        return;
      } // else
      
    } // if
    
  } // else if
  else {
    temp = GetToken();
    ErrorHandler( temp.str, 2 );
  } // else
  
} // Not_ID_StartFactor()

void ArithExp( bool & correct, Token & token ) {
  // <ArithExp> ::= <Term> { '+' <Term> | '-' <Term> }
  if( gDEBUGMODE ) {
    cout << "ArithExp\n";
  } // if
  
  Token temp, token1;
  Initialize(temp);
  Initialize(token1);
  
  Term( correct, token );
  if( !correct ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while( gUnDealToken.str == "+" || gUnDealToken.str == "-" ) {
    temp = GetToken(); // '+' || '-'
    Term( correct, token1 );
    if( !correct ) {
      // 有 '+' 或 '-' 一定要有Term
      ErrorHandler( token1.str, 2 );
    } // if
    
    if( temp.str == "+" ) {
      token.value = token.value + token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
      
    } // if
    else {
      token.value = token.value - token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
     
    } // else
    
    PeekToken();
  } // while

} // ArithExp()

void Term( bool & correct, Token & token ) {
  // <Term> ::= <Factor> { '*' <Factor> | '/' <Factor> }
  if( gDEBUGMODE ) {
    cout << "Term\n";
  } // if
  Token temp, token1;
  Initialize(temp);
  Initialize(token1);
  
  Factor( correct, token );
  if( !correct ) {
    ErrorHandler( token.str, 2 );
  } // if
  
  PeekToken();
  while( gUnDealToken.str == "*" || gUnDealToken.str == "/" ) {
    temp = GetToken(); // '*' || '/'
    Factor( correct, token1 );
    if( !correct ) {
      // 有 '*' 或 '/' 一定要有Factor 
      ErrorHandler( token1.str, 2 );
    } // if
    
    if( temp.str == "*" ) {
      token.value = token.value * token1.value;
      if( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if 
      
    } // if
    else {
      if ( token1.value == 0 ) {
        ErrorHandler( "", 0 );
      } // if
      
      token.value = token.value / token1.value;
      if ( token1.value_type == FLOAT ) {
        token.value_type = FLOAT;
      } // if
      
      if ( token.value_type == INT && token.value >= 0 ) {
        token.value = floor( token.value );
      } // if
      else if ( token.value_type == INT && token.value < 0 ) {
        token.value = ceil( token.value );
      } // else

    } // else
    
    PeekToken();
  } // while

} // Term()

void Factor( bool & correct, Token & token ) {
  // <Factor> ::= IDENT | [SIGN] NUM | '(' <ArithExp> ')'
  if( gDEBUGMODE ) {
    cout << "FACTOR\n";
  } // if
  Token temp;
  Initialize(temp);
  bool defined = false;
  correct = true;
  
  PeekToken();
  if( gUnDealToken.type == IDENT ) {
    token = GetToken();
    for( int i = 0; i < gIDTable.size(); i++ ) {
      if( token.str == gIDTable.at(i).str ) {
        token = gIDTable.at(i);
        defined = true;
      } // if
      
    } // for
    
    if( !defined ) {
      //correct = false;
      ErrorHandler( token.str, 3 );
    } // if
    
    return;
  } // if
  else if( gUnDealToken.str == "+" ) {
    // 遇到sign，則一定要有NUM 
    temp = GetToken(); // '+'
    token = GetToken();
    if( token.type != NUM ) {
      // 發生unexpected，讀掉剩餘部分
      //correct = false;
      ErrorHandler( token.str, 2 );
      //return;
    } // if
    else {
      //正確無誤
      correct = true;
      return;
    } // else

  } // else if
  else if( gUnDealToken.str == "-" ) {
    // 遇到sign，則一定要有NUM 
    temp = GetToken(); // '-'
    token = GetToken();
    
    if( token.type != NUM ) {
      // 發生unexpected，讀掉剩餘部分 
      //correct = false;
      ErrorHandler( token.str, 2 );
      //return;
    } // if
    else {
      token.value = 0 - token.value;
      correct = true;
      return;
    } // else
    
  } // else if
  else if( gUnDealToken.type == NUM ) {
    token = GetToken();
    correct = true;
    return;
  } // else if
  else if( gUnDealToken.str == "(" ) {
    // '(' <ArithExp> ')' 
    temp = GetToken(); // '('
    ArithExp( correct, token );
    if( correct ) {
      temp = GetToken(); // ')'
      if( temp.str != ")" ) {
        //correct = false; 
        ErrorHandler( temp.str, 2 );
      } // if
      else {
        correct = true;
        return;
      } // else
      
    } // if
    
  } // else if
  else {
    //correct = false; 
    ErrorHandler( gUnDealToken.str, 2 );
  } // else

} // Factor()

int main() {
  
  Token token;
  bool programContinue = true;
  Initialize( gUnDealToken );
  Initialize( token );
  cout << "Program starts...\n";
  while( programContinue == true ) {
    Command(programContinue);
  } // while
  cout << "> Program exits...\n";
} // main()

