# nush - Northeastern University Shell
This shell can run any executable program on the command line, and it also supports several built-in commands and operations.

## Built-in commands
  * change directory: `cd`
  * exit program: `exit`

## Supported operations
  * input redirection: `<`, `>`
  * pipe: `|`
  * background processes: `&`
  * logical and: `&&`
  * logical or: `||`
  * command separator: `;`
  * variable assignment: `=`

## How to run
  1. Clone this repository
  2. Navigate into the repository and run `make`
  3. Run ./nush by:
      * Entering `./nush tests/{test-name.sh}`
      * Or entering `./nush` and providing arguments via `$nush` prompt
