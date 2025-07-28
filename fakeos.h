#pragma once

#define PAGE_SIZE (4096)
#define ERROR (0)
#define TIMEOUT (0xEFFFFFFF)

// TODO: in real life this is the actual memory page ID not a char*
typedef char* Page; // one page of memory
typedef int LinkID; // 32 bit socket ID
typedef long long int Ticks; // clock tick count

Ticks ticks();  // current uptime in ticks
Ticks second(); // ticks per second so you can sleep(3*second())
void  sleep(Ticks ticks);

// TODO:
// creating a linked list of pages and passing them to syscalls does not work !!!
// it has to be a contiguous chunk of memory
// this might require passing a count of pages to allocate to page() or using order() syscall

Page page();     // Allocates a new page.  0 indicates an error
void drop(Page p); // Deletes a page
// insert() Orders pages in physical memory one after another to form contiguous chunk
// ordering pages like that allows resizing vectors without realloc()
// it can allow building buffers page by page as data is being read
// false can be returned if inserting pages will override another page's physical address ???
// page() can take parameters to keep buffers, heap and stack separately.
// in theorystack frame could start on new page every time.
bool insert(Page p1, Page p2); 
char* address(Page p); // return address of this page

bool run(Page firstPage, unsigned long long size); // start a process from the image loaded in memory
void done(int errlevel); // terminate the process.  This function does not return.

// Create a client or a server.  LinkID 0 indicates an error
// url is a page containing a null terminated string (c string) starting at offset 0
// url format is protocol://ip_or_name:port/cmd_line_parameters
LinkID link(Page url);    // Page url is valid after link() returns
void unlink(LinkID link); // close the link
Page info(LinkID link);   // information about a link (url or remote IP:port, state)

// if write's destination is a local process,
// "kernel" moves this page to another process's memory space for it to read()
// If 0 is returned, page becomes invalid after write(), otherwise page is returned back
Page write(LinkID link, Page page);
Page read(LinkID link); // eventually read page has to be deleted via drop()
// TODO: think about how to return the number of available bytes on Page

// Returns the first LinkID available for read/write
// Pass in positive descriptors to check if they can be read from
// Pass in negative discriptors to check if they can be written to
// If returned LinkID is positive, it's available for read / accept ???
// If returned LinkID is negative, it's available for write
// If any LinkIDs in links parameter get an exception, 0 is returned
// Zero can be followed up by info() on all LinkIDs
// LinkID in links can be zero.  Zero LinkIDs are skipped.
// Special value of TIMEOUT is returned when timeout has been reached
LinkID select(Page links, int count, Ticks timeout);
// TODO: use a 16 bit LinkID to increase how many will fit on one Page from 1024 to 2048 ???
// TODO: use multiple pages through a pointer on each page?

// These are user space helper functions.  Syscall does not occur when they are called
LinkID forRead(LinkID lid)  { return lid>0 ? lid:-lid; } // use to select() a LinkID for read
LinkID forWrite(LinkID lid) { return lid<0 ? lid:-lid; } // use to select() a LinkID for write
LinkID canRead(LinkID lid)  { return lid>0 ? lid:0; } // returns LinkID if lid is readable, otherwise 0
LinkID canWrite(LinkID lid) { return lid<0 ? lid:0; } // returns LinkID if lid is writeable, otherwise 0
