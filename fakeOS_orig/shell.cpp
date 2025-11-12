// Shell or telnetd or ssh or init.d or inetd (depends how you look at it) for fakeOS
// of course this is also fake... since it uses vectors (memory allocated using new() etc...)
// using c++ just for the vector
#include "fakeos.h"
#include <vector>
using namespace std;

// format url from what was read from the user through read() into format acceptable by link()
Page formatUrl(Page url){
    if(url){
        // url was read from the user.  Format it for using in link(url)
    }
    return url;
}

// takes a Page with a null terminated URL string on it
// goes to that Reliable UDP URL and downloads the executable into vector "pages".
size_t readImage(Page url, vector<Page>& pages){
    // open connection to url
    // read file into pages
    return 0;
}

// allocates stack
// calls various things to initialize statics
// creates a thread
// calls main with parameters console, selfUrl on that thread
bool spawn(vector<Page>& pages, size_t size, LinkID console, Page selfUrl){
    return 0;
}

// copy null terminated string from src to dest
void strcpy(char* dest, const char* src){}


int main(int argc, char* argv[]){
    Page serverUrl = page();
    strcpy(serverUrl, "RUDP://:1234"); // Reliable UDP 

    while(1){
        LinkID conn = link(serverUrl); // we want to block untill the next request
        if(!conn){ continue; }// TODO: log error to console, if there is a console :)

        Page programUrl = read(conn);
        if(!programUrl){ 
            // TODO: send error to conn
            unlink(conn);
            continue;
        }

        if( !formatUrl(programUrl) ){
            // TODO: send error to conn
            unlink(conn);
            drop(programUrl);
            continue;
        }

        vector<Page> pages;
        size_t size = readImage(programUrl, pages);
        if(size <= 0){
            // TODO: send error to conn
            unlink(conn);
            drop(programUrl);
            continue;
        }

        if( spawn(pages, size, conn, programUrl) ){ // could run out of memory...
            // TODO: send error to conn
            unlink(conn);
            drop(programUrl);
            continue;
        }
    }
}
