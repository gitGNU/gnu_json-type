// Copyright (C) 2016  Stefan Vargyas
// 
// This file is part of Json-Type.
// 
// Json-Type is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Json-Type is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Json-Type.  If not, see <http://www.gnu.org/licenses/>.

json
   : value +
   ;
value
   : string
   | number
   | object
   | array
   | "true"
   | "false"
   | "null"
   ;
object
   : "{" [ pair ("," pair)* ] "}"
   ;
pair
   : string ":" value
   ;
array
   : "[" [ value ("," value)* ] "]"
   ;
string
   : '"' ( <any UNICODE character except " or \ or control character> | '\' ["\/bfnrt] | "\\u" hex hex hex hex )* '"'
   ;
number
   : integer [ fractional ] [ exponent ]
   ;
integer
   : [ "-" ] ( "0" | digit1 [ digits ] )
   ;
digits
   : digit +
   ;
digit
   : [0-9]
   ;
digit1
   : [1-9]
   ;
fractional
   : "." digits
   ;
exponent
   : [eE] [ [+-] ] digits
   ;
hex: [0-9a-fA-F]
   ;
WSP: [\ \t\n\r]
   ;


