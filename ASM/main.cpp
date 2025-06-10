#include<iostream>
#include"Lexical_analysis.h"
#include "FirstPassListing.h"
#include "SecondPassListing.h"

int main() 
{
        analyzeFile("test1.asm");
        std::cout << "Lexical analysis completed. Output written to 'lexemes_table.txt'" << std::endl;
        FirstPassListing firstPassListing;

        SecondPassListing secondPassListing(firstPassListing);
        firstPassListing.generateListing("test1.asm", "first_pass_listing.txt");
        std::cout << "First pass listing completed. Output written to 'first_pass_listing.txt'" << std::endl;
        secondPassListing.generateListing("test1.asm", "second_pass_listing.lst");
        std::cout << "Second pass listing completed. Output written to 'second_pass_listing.lst'" << std::endl;
        

        return 0;
    }

