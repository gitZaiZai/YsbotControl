#ifndef Journal_h
#define Journal_h

#include <queue>
#include <string>
#include <fstream>
#include "ConfigReader.h"
 
class Journal
{
public:
	 
    Journal() throw ();
     ~Journal() throw ();
    
    void set_mode (  ConfigReader&, std::string wpath = "") throw ();
    void set_verbosity (int) throw ();
    void set_memory_mode () throw (); 
    void set_stream_mode (std::ostream&) throw ();
    void clear_buffer () throw ();
    void write_buffer (std::ostream&) throw ();
    
    void error (const char*, unsigned int, const char*) throw ();
    void warning (const char*, unsigned int, const char*) throw ();
    void message (const char*) throw ();    
 
    static Journal the_journal;
 
private:
	int						verbosity;                    // 1, 2, 3 
    std::queue<std::string> message_buffer;    
    std::ostream*			output_stream_pointer;   
    std::ofstream*			output_file_pointer;   
    std::string				flush_output;         
};

#define JERROR(t)		Journal::the_journal.error(__FILE__, __LINE__,t)
#define JWARNING(t)		Journal::the_journal.warning(__FILE__, __LINE__,t)
#define JMESSAGE(t)		Journal::the_journal.message(t)
#define JSPEECHOUT(t)	Journal::the_journal.sound_message(t)

#endif