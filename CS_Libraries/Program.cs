using static System.Console;
using Lib.Error;

namespace CS_Libraries
{
    class Program
    {
        public static void LibraryExceptionHandler(LibraryException e)
        {
            WriteLine(e.Message);
        }

        public static void Main()
        {
            LibraryExceptionHandlers.LibraryExceptionHandler += LibraryExceptionHandler;

            try
            {
                throw new LibraryException();
            }
            catch (LibraryException e)
            {
                
            }
        }
    }
}