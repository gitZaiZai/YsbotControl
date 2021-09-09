#include "StdAfx.h"
#include "Journal.h"
#include "Time.h"
#include <cmath>
 
using namespace std;

Journal Journal::the_journal;

Journal::Journal() throw () : verbosity (1), output_stream_pointer (NULL), output_file_pointer (NULL) {;}
 
Journal::~Journal() throw () 
{
  if (output_stream_pointer)
    output_stream_pointer->flush();
  if (output_file_pointer)
    delete output_file_pointer;
  if (!message_buffer.empty())
  {
    if (flush_output.length()==0)
      write_buffer (std::cerr);
    else 
	{
      ofstream f (flush_output.c_str());
      if (f)
        write_buffer (f);
      else
        write_buffer (std::cerr);
    }
  }
} 

void Journal::set_mode ( ConfigReader& vread, std::string wpath ) throw ( ) 
{
  int new_verbosity;
  if (vread.get ("Journal::verbosity", new_verbosity)>0)
    verbosity = new_verbosity;
  string confline;

  if( vread.get ("Journal::output", confline)<=0 )
  {
    AfxMessageBox(_T("Journal::output"));
  }
  if (confline == "Journal::memory")
    set_memory_mode();
  else if (confline == "stdout")
	  set_stream_mode (std::cout);
  else if (confline == "stderr")
    set_stream_mode (std::cerr);
  else 
  {
    confline = wpath + confline;
    ofstream* errfile = new ofstream (confline.c_str());
    if (!(*errfile)) 
	{
       delete errfile;
	  AfxMessageBox(_T("Journal::output"));
    }
    if (output_stream_pointer)
      output_stream_pointer->flush();
    if (output_file_pointer)
      delete output_file_pointer;
	
    output_stream_pointer = errfile;
    output_file_pointer   = errfile;
  }

  if( vread.get ("Journal::flush", confline) <=0 )
    flush_output = "";
  else
    flush_output = confline;
}

void Journal::set_verbosity (int i) throw () 
{
  verbosity=i;
}

void Journal::set_memory_mode () throw () 
{
  if (output_stream_pointer)
    output_stream_pointer->flush();
  output_stream_pointer = NULL;
  if (output_file_pointer) 
  {
    delete output_file_pointer;
    output_file_pointer = NULL;
  }
}

void Journal::set_stream_mode (std::ostream& os) throw () 
{
  if (output_stream_pointer)
    output_stream_pointer->flush();
  output_stream_pointer = &os;
  if (output_file_pointer) 
  {
    delete output_file_pointer;
    output_file_pointer=NULL;
  }
}

void Journal::clear_buffer () throw () 
{
  while (!message_buffer.empty())
    message_buffer.pop();
}

void Journal::write_buffer (std::ostream& os) throw () 
{
  while (!message_buffer.empty()) {
    os << message_buffer.front() << "\n";
    message_buffer.pop();
  }
}

namespace 
{
  
  inline string uint2str (unsigned int iii) 
  {
     if( iii == 0 )
       return string("0");

     const unsigned int slen = static_cast<unsigned int>(floor(log(static_cast<double>(iii))/log(10.0))+1);
     string res( slen,'0' );
     
	 for (int j=slen-1; j>=0; j--) 
	 {
       res[j]=static_cast<char>(iii%10+'0');
       iii=iii/10;
     }
     return res;
  }
}

void Journal::error (const char* fname, unsigned int lnum, const char* errstr) throw () 
{
  Ysbot::Time ctime;
  if( output_stream_pointer )
    (*output_stream_pointer) << ctime << " Error in " << fname <<  ", " << lnum << ": " << errstr << "\n";
  else 
  {
    try
	{  
      message_buffer.push( uint2str(ctime.get_msec()/1000)+string(".") + 
		                   uint2str(ctime.get_msec()%1000)+string(" Error in ") +
						   string(fname)+string(", ") +uint2str(lnum)+string(": ")+string(errstr) );
    }
    catch(bad_alloc&)
	{
      std::cerr << "Yasierr in " << __FILE__ << ", " << __LINE__ << "\n";
      std::cerr << ctime << " Error in " << fname <<  ", " << lnum << ": " << errstr << "\n";
    }
  }
} 

void Journal::warning (const char* fname, unsigned int lnum, const char* errstr) throw () {
  if (verbosity>=2) 
  {
    Ysbot::Time ctime;
    if( output_stream_pointer )
      (*output_stream_pointer) << ctime << " Warning in " << fname <<  ", " << lnum << ": " << errstr << "\n";
    else 
	{
      try
	  { 
        message_buffer.push(uint2str(ctime.get_msec()/1000)+string(".")+
			uint2str(ctime.get_msec()%1000)+string(" Warning in ")+
			string(fname)+string(", ")+uint2str(lnum)+string(": ")+
			string(errstr));
      }
	  catch(bad_alloc&)
	  {
        
        std::cerr << "Yasierr in " << __FILE__ << ", " << __LINE__ << "\n";
        std::cerr << ctime << " Warning in " << fname <<  ", " << lnum << ": " << errstr << "\n";
      }
	}
  }
}

void Journal::message (const char* errstr) throw ()
{
  if (verbosity>=3) 
  {
    if (output_stream_pointer)
      (*output_stream_pointer) << errstr << "\n";
    else 
	{
      try
	  {
        message_buffer.push(string(errstr));
      }
	  catch(bad_alloc&)
	  {
         
        std::cerr << "Yasierr  in __FILE__, __LINE__\n";
        std::cerr << errstr << "\n";
      }
	}
  }
}


 