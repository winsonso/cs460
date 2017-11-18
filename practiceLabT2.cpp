/***********************************************************************
* Program:
*    Lab 00T2, Practice Lab Submission
*    Brother Jones, CS460
* Author:
*    Winson So
* Summary:
*    The program is to count the number of strings entered by the user
*    at the command line that have the first letter of the first word
*    capitalized before a blank line is entered by the user
************************************************************************/
/*****************************************************************************
*
* Changes made to my code for the re-submission:
* Nothing changes.
*
*****************************************************************************/

#include <iostream>
using namespace std;

/**********************************************************************
 * This function will ask the user to input a string. It will count
 * the number of times the user input a string with the first uppercase
 * letter, it then return the result.
 ***********************************************************************/
int main()
{
   string answer;
   int counter = 0;
   // terminate the program if user inputs nothing
   do
   {
      cout << "Enter a string: ";
      getline(cin,answer);
      
      if (isupper(answer[0]))
      {
         counter++;
      }
   } while (answer.length() != 0);
   
   // gives the correct output on different conditions.
   if (counter == 1)
   {
      cout << "One string starting with an upper case letter was seen.\n";
   }
   else if (counter > 1)
   {
      cout << "You entered ";
      cout << counter;
      cout << " strings that started with an upper case letter.\n";
   }
   else
   {
      cout << "No strings starting with an upper case letter were seen.\n";
   }
   return 0;
}
