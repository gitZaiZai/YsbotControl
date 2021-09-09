
#ifndef _Tribots_ConfigReader_h_
#define _Tribots_ConfigReader_h_

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <stack>
#include "YsibotsException.h"

  class ConfigReader {
  struct CmdLineShortcut {
    std::string short_key;
    std::string long_key;
    bool has_arg;
  };
  public:
   
    ConfigReader (unsigned int=0, char ='#', char ='=') throw ();
    ConfigReader (const ConfigReader&) throw (std::bad_alloc);
    ~ConfigReader () throw ();
  
    void add_command_line_shortcut (const char*, const char*, bool);

    bool append_from_file (const char*, bool=true) throw (std::bad_alloc);
  
    bool replace_config_file (const char*, bool =true) throw (std::bad_alloc);
    bool replace_config_file (const char*, const std::vector<std::string>&, bool =true) throw (std::bad_alloc);

	bool replace_config_file_win(const char*,  bool =true) throw (std::bad_alloc); // Dan test

    const std::vector<std::string>& list_of_sources () const throw ();


    int get (const char*, int&) const throw (std::bad_alloc);                  ///< int lesen
    int get (const char*, unsigned int&) const throw (std::bad_alloc);         ///< unsigned int lesen
    int get (const char*, long int&) const throw (std::bad_alloc);             ///< long int lesen
    int get (const char*, unsigned long int&) const throw (std::bad_alloc);    ///< unsigned long int lesen
    int get (const char*, double&) const throw (std::bad_alloc);               ///< double lesen
    int get (const char*, float&) const throw (std::bad_alloc);                ///< double lesen
    int get (const char*, char&) const throw (std::bad_alloc);                 ///< char lesen
    int get (const char*, bool&) const throw (std::bad_alloc);                 ///< bool lesen (1/0/true/false)
    int get (const char*, std::string&) const throw (std::bad_alloc);          ///< string (ein Wort) lesen
    int getline (const char*, std::string&) const throw (std::bad_alloc);          ///< string (ganze Zeile) lesen

    int get (const char*, std::vector<int>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<unsigned int>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<long int>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<unsigned long int>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<double>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<float>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<char>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<bool>&) const throw (std::bad_alloc);
    int get (const char*, std::vector<std::string>&) const throw (std::bad_alloc);

    void set (const char*, int) throw (std::bad_alloc);                  ///< int schreiben
    void set (const char*, unsigned int) throw (std::bad_alloc);         ///< unsigned int schreiben
    void set (const char*, long int) throw (std::bad_alloc);             ///< long int schreiben
    void set (const char*, unsigned long int) throw (std::bad_alloc);    ///< unsigned long int schreiben
    void set (const char*, double) throw (std::bad_alloc);               ///< double schreiben
    void set (const char*, float) throw (std::bad_alloc);                ///< double schreiben
    void set (const char*, char) throw (std::bad_alloc);                 ///< char schreiben
    void set (const char*, bool) throw (std::bad_alloc);                 ///< bool schreiben (true/false)
    void set (const char*, const std::string&) throw (std::bad_alloc);   ///< string (ein Wort) schreiben
    void set (const char*, const char*) throw (std::bad_alloc);          ///< string (ein Wort) schreiben

    void set (const char*, const std::vector<int>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<unsigned int>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<long int>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<unsigned long int>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<double>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<float>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<char>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<bool>&) throw (std::bad_alloc);
    void set (const char*, const std::vector<std::string>&) throw (std::bad_alloc);

     bool write (std::ostream&, const char*) const throw ();

     void write (std::ostream&) const throw ();

     void write_section (std::ostream&, const char*) const throw ();

	 bool write_config_file (const char*, bool =true) throw (std::bad_alloc);
	 bool write_all() throw (std::bad_alloc);

	 const char* getenv() const throw ();
	 const char* get_config_path() const throw ();

   private:
	  bool delete_config_bak(const char* );

  protected:
    struct KVPair;
    std::vector<KVPair> key_value_table;         
    const unsigned int verbosity;               
    const char comment_char;                   
    const char assign_char;                     
    std::stack<std::string> directory_stack;    
    std::vector<std::string> files;              
    std::string home_path;                     
	std::string conf_name;

    std::vector<CmdLineShortcut> shortcuts;     
    unsigned int unknown_parameter_counter;     

    bool find_first (std::string& value, const std::string& key) const;   
    bool find_all (std::deque<std::string>& values, const std::string& key) const;   
    
    bool file_input (const char*, const char*, const std::vector<std::string>&, bool, std::string&) throw (std::bad_alloc);

    void insert_pair (const std::string&, const std::string&) throw (std::bad_alloc);

  };
 
#endif
