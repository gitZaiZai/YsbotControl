#include "StdAfx.h"
#include "ConfigReader.h"
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <cstdio>
#include "DirectoryOperate.h"
#include "stringconvert.h"

using namespace std;
 
struct ConfigReader::KVPair {
	string key;
	string value;

	KVPair () {;}
	KVPair (const KVPair& kv) : key (kv.key), value (kv.value) {;}
	const KVPair& operator= (const KVPair& kv) {
		key=kv.key;
		value=kv.value;
		return *this;
	}
	bool operator== (const KVPair& kv) const throw () { return (key==kv.key); }
	bool operator< (const KVPair& kv) const throw () { return (key<kv.key); }
};


namespace {
	string next_word (const string& str, unsigned int& endpos, unsigned int firstpos) {
		unsigned int first_non_white=firstpos;
		if (str.length()<=firstpos) {
			endpos=firstpos;
			return string("");
		} else {
			endpos = firstpos+1;
			bool was_whitespace = (isspace (str [firstpos]) !=0 ) ? true : false;
			while (endpos<str.length()) {
				if (isspace(str [endpos])) {
					if (!was_whitespace) {
						return str.substr(first_non_white,endpos-first_non_white);
					}
				} else {
					if (was_whitespace) {
						was_whitespace=false;
						first_non_white=endpos;
					}
				}
				endpos++;
			}
			// Stringende erreicht
			return (str.substr(first_non_white,endpos-first_non_white));
		}
	}

	// pruefen, ob string nur aus whitespace besteht
	bool is_whitespace (const string& s) {
		unsigned int n=s.length();
		for (unsigned int i=0; i<n; i++)
			if (!isspace(s[i]))
				return false;
		return true;
	}

	// Whitespace am Anfang und Ende eines String entfernen
	string remove_trailing_whitespace (const string& src) {
		int n=src.length();
		int first_character=0;
		int last_character=n-1;
		for (int i=0; i<n; i++)
			if (isspace (src[i]))
				first_character++;
			else
				break;
		for (int i=n-1; i>=0; i--)
			if (isspace (src[i]))
				last_character--;
			else
				break;
		if (first_character>last_character)
			return string("");
		return src.substr (first_character, last_character-first_character+1);
	}

	// in einer sortierten container<element> suchen
	// Arg1: Position des Wertes (oder wo der Wert eingefuegt werden muesste),
	// Arg2: Die deque
	// Arg3: Der Schluessel
	// Ret: Element gefunden?
	template<class C, class T>
	bool find_sorted (unsigned int& rv, const C& container, const T& element) {
		unsigned int right=container.size();
		unsigned int left=0;
		unsigned int searchpos;

		if (right==0) {
			// empty container
			rv=0;
			return false;
		}
		while (true) {
			if (left+1>=right) {
				if (container [left]==element) {
					// element found
					rv=left;
					return true;
				} else {
					// element not in container
					if (container[left]<element)
						rv=left+1;
					else
						rv=left;
					return false;
				}
			} else {
				searchpos=(left+right)/2;   // left>right => searchpos<left => container[searchpos] exists
				if (element<container[searchpos]) {
					// search on the left hand side of searchpos
					right=searchpos;
				} else {
					// search on the right hand side of searchpos
					left=searchpos;
				}
			}
		}
	}

	void extract_path_from_filename (std::string& path, std::string& filename, const std::string& line) {
		if (line.length()==0) {
			filename = path = "";
			return;
		}
		unsigned int i=line.length()-1;
		while (i>0)
			if (line[i]=='/')
				break;
			else
				i--;
		if (line[i]=='/') {
			path = line.substr (0,i+1);
			filename = line.substr (i+1, line.length()-i-1);
			return;
		}
		path = "";
		filename = line;
		return;
	}

	void extract_path_from_filename_win (std::string& path, std::string& filename, const std::string& line) {
		if (line.length()==0) {
			filename = path = "";
			return;
		}
		unsigned int i=line.length()-1;
		while (i>0)
			if (line[i]=='\\')
				break;
			else
				i--;
		if (line[i]=='\\') {
			path = line.substr (0,i+1);
			filename = line.substr (i+1, line.length()-i-1);
			return;
		}
		path = "";
		filename = line;
		return;
	}

	string replace_all (const string& s1, const string& s2, const string& s3) { // durchsucht s1 sequentiell nach Vorkommen von s2 und ersetzt diese durch s3
		string res = s1;
		unsigned int pos = 0;
		while ((pos = res.find (s2, pos))<res.length()) {
			res.replace (pos, s2.length(), s3);
			pos+=s3.length();
		}
		return res;
	}

} // namespace


ConfigReader::ConfigReader (unsigned int vb, char cc, char ac) throw () : verbosity (vb), comment_char (cc), assign_char (ac) {
	directory_stack.push (string(""));
	home_path = "";
	TCHAR szFilePath[MAX_PATH + 1];
	if( 1 )
	{
		GetCurrentDirectory( MAX_PATH, szFilePath );     // 当前工作路径
	}
	else
	{
		GetModuleFileName( NULL, szFilePath, MAX_PATH ); // exe文件所在的路径
		(_tcsrchr(szFilePath,_T('\\')))[1] = 0;
	}
	home_path = Unicode2Ansi( wstring(szFilePath) + _T("\\") );
	unknown_parameter_counter=0;
}

ConfigReader::~ConfigReader () throw () {;}


// -------------------- Auslesen der Dateien: ------------------
bool ConfigReader::replace_config_file_win(const char* name, bool recursive) throw (std::bad_alloc) {
	std::string fname = std::string(name);
	int pos;
	char b = '\\';
 	pos = fname.find(b); 
 	while( pos != -1 )
 	{
 		fname.replace(pos,1,"/"); 
 		pos = fname.find(b); 
	} 

	return replace_config_file(fname.c_str(), recursive);
}

bool ConfigReader::replace_config_file (const char* name, bool recursive) throw (std::bad_alloc) {
	std::string prefix = "";
	std::vector<std::string> emptylist;
	return file_input (".bak", name, emptylist, recursive, prefix);
}

bool ConfigReader::replace_config_file (const char* name, const std::vector<std::string>& repkeys, bool recursive) throw (std::bad_alloc) {
	std::string prefix = "";
	return file_input (".bak", name, repkeys, recursive, prefix);
}

bool ConfigReader::append_from_file (const char* name, bool recursive) throw (std::bad_alloc) {
	std::string prefix = "";
	std::vector<std::string> emptylist;
	conf_name = home_path + std::string(name);
	return file_input (NULL, name, emptylist, recursive, prefix);
}

bool ConfigReader::file_input (const char* baksuffix, const char* name, const std::vector<std::string>& repkeys, bool recursive, std::string& prefix) throw (std::bad_alloc) {
	bool replace_mode = (baksuffix!=NULL);
	bool replace_all_keys = (repkeys.size()==0);

	// aktuelles Verzeichnis feststellen und in den Directory-Stack schieben
	string cpath = directory_stack.top();
	string fpath, fname, npath;
	extract_path_from_filename (fpath, fname, name);
	if (fpath.length()>0 && fpath[0]=='/')
		npath = fpath;
	else
		npath = cpath+fpath;
	directory_stack.push (npath);

	ifstream* src = NULL;
	ofstream* dest = NULL;
	bool filesuccess = true;
	std::string filename = npath+fname;

	if (replace_mode) {
		// Quelle nach Quelle.baksuffix umbenennen, Quelle und Ziel oeffnen
		string old_fname = filename+string(baksuffix);
		filesuccess &= (rename (filename.c_str(), old_fname.c_str())==0);
		src = new ifstream (old_fname.c_str());
		dest = new ofstream (filename.c_str());
		filesuccess &= ((*src)!=NULL);
		filesuccess &= ((*dest)!=NULL);
	} else {
		// Quelle in die Dateiliste uebernehmen, Quelle oeffnen
		files.push_back (filename);
		src = new ifstream (filename.c_str());
		filesuccess &= ((*src)!=NULL);
	}

	// waren die Dateioperationen nicht erfolgreich?
	if (!filesuccess) {
		if (verbosity>=1)
			cerr << "ConfigReader Error: cannot handle file " << filename << '\n';
		directory_stack.pop();
		return false;
	}
	if (verbosity>=2) {
		if (replace_mode)
			cerr << "ConfigReader: copy from file " << npath << filename << baksuffix << " to " << npath << filename << '\n';
		else
			cerr << "ConfigReader: read from file " << npath << filename << '\n';
	}


	// Streams auslesen (rekursiv)
	bool success = true;

	// Zeilenweise auslesen:
	while (!(src->eof())) {
		string oline;
		std::getline (*src, oline);
		if (src->eof() && oline.length()==0)
			break; // Dateiende erreicht
		unsigned int p=oline.find (comment_char);
		unsigned int n=oline.length();
		string line = oline;
		string comment = "";
		if (p<n) {
			// Kommentare abschneiden
			line.erase (p, n-p);
			comment = oline.substr (p, n-p);
			n=p;
		}
		// Zwischenstand:
		// 'line' enthaelt die Eingabezeile ohne Kommentare
		// 'comment' enthaelt den Kommentaranteil der Zeile
		// line+comment ergibt die Originalzeile
		// 'n' ist die Laenge von 'line'

		if (n==0) {
			// leere Zeilen oder reine Kommentarzeilen unveraendert uebernehmen
			if (replace_mode)
				(*dest) << comment << '\n';
			continue;
		}

		if (line[0]=='+') {
			// pruefen, ob eine weitere Konfigurationsdatei eingebunden werden soll
			unsigned int pos0=1;
			if (line[1]=='+')
				pos0=2;
			unsigned int dont_care;
			string fname = replace_all(replace_all(next_word (line, dont_care, pos0),string("$PATH"),directory_stack.top()),string("$HOME"),home_path);
			if (recursive) {
				std::string empty_string ("");
				success &= file_input ((replace_mode ? baksuffix : NULL), fname.c_str(), repkeys, true, (pos0==1 ? empty_string : prefix));
			}
			if (replace_mode)
				(*dest) << oline << '\n';
			continue;
		}

		if (line[0]=='[') {
			// pruefen, ob ein neuer Block definiert wird
			p=line.find (']');
			if (p<n) {
				if (p==1) {
					prefix = "";
					if (verbosity>=3)
						cerr << "ConfigReader: read from empty block\n";
				} else {
					prefix = line.substr (1,p-1)+string("::");
					if (verbosity>=3)
						cerr << "ConfigReader: read from block " << line.substr (1,p-1) << '\n';
				}
				if (replace_mode)
					(*dest) << oline << '\n';
			} else {
				success=false;
				if (verbosity>=1)
					cerr << "ConfigReader Error: invalid block definition " << line << '\n';
				if (replace_mode)
					(*dest) << oline << '\n';
			}
			continue;
		}

		// normale Zeile auswerten:
		p=line.find (assign_char);
		if (p<n) {
			KVPair new_pair;
			string pure_key = remove_trailing_whitespace (line.substr (0,p));
			new_pair.key = prefix+pure_key;  // Schluessel extrahieren
			new_pair.value = replace_all (replace_all (remove_trailing_whitespace (line.substr (p+1, n-p-1)),string("$PATH"),directory_stack.top()), string("$HOME"),home_path);  // Wert extrahieren und Variablen ersetzen
			unsigned int pos;
			if (verbosity==3) 
				cerr << "ConfigReader: read line " << new_pair.key << '\n';
			// nach dem Schluessel in der Liste suchen:
			if (find_sorted (pos, key_value_table, new_pair)) {
				if (replace_mode) {
					// Zeile mit neuem Wert schreiben, falls moeglich
					if (key_value_table[pos].value!=new_pair.value && (replace_all_keys || (find(repkeys.begin(), repkeys.end(), new_pair.key)!=repkeys.end()))) {
						(*dest) << pure_key << '\t' << assign_char << ' ' << key_value_table[pos].value << (comment.length()>0 ? " " : "") << comment << '\n';
						if (verbosity>=1)
							cerr << "ConfigReader: replace line " << new_pair.key << " | " << new_pair.value << '\n';
					} else {
						(*dest) << oline << '\n';
						if (verbosity>=3)
							cerr << "ConfigReader: keep line " << new_pair.key << " | " << new_pair.value << '\n';
					}
				} else {
					// vorhanden Eintrag ueberschreiben:
					key_value_table[pos]=new_pair;
					if (verbosity>=4)
						cerr << "ConfigReader: replace value " << new_pair.key << " | " << new_pair.value << '\n';
				}
			} else {
				// den Schluessel in der Liste nicht gefunden
				if (replace_mode) {
					// Zeile beibehalten
					if (verbosity>=3)
						cerr << "ConfigReader: keep line " << new_pair.key << " | " << new_pair.value << '\n';
					(*dest) << oline << '\n';
				} else {
					// neuen Eintrag erzeugen
					if (verbosity>=4)
						cerr << "ConfigReader: insert value " << new_pair.key << " | " << new_pair.value << '\n';
					key_value_table.insert (key_value_table.begin()+pos, new_pair);
				}
			}
		} else {
			if (replace_mode)
				// nicht interpretierbare Zeile unveraendert uebernehmen
				(*dest) << oline << '\n';
		}
	}
	// Ende Streams auslesen

	directory_stack.pop();
	if (src)
		delete src;
	if (dest) {
		dest->flush();
		delete dest;
	}
	return success;
}




// ---------------- suchen nach Schluesseln: ------------------------

bool ConfigReader::find_first (string& value, const string& key) const {
	unsigned int p;
	KVPair kv;
	kv.key=key;
	bool found = find_sorted (p, key_value_table, kv);
	if (found) {
		unsigned int dummy;
		value = next_word (key_value_table [p].value, dummy, 0);
	}
	if (!found && verbosity>=1)
		cerr << "ConfigReader Error: key " << key << " not found\n";
	return found;
}

bool ConfigReader::find_all (deque<string>& values, const string& key) const {
	values.clear();
	unsigned int p;
	KVPair kv;
	kv.key=key;
	bool found = find_sorted (p, key_value_table, kv);
	if (found) {
		unsigned int n = key_value_table [p].value.length();
		string word;
		unsigned int q=0;
		while (q<n) {
			word = next_word (key_value_table [p].value, q, q);
			if (!is_whitespace (word))
				values.push_back (word);
		}
	}
	if (!found && verbosity>=1)
		cerr << "ConfigReader Error: key " << key << " not found\n";
	return found;
}



// ------------------- get(.)-Funktionen: ------------------------

namespace {

	// einige Konvertierungsfunktionen:
	bool seval (long int& d, const string& s) {
		char* end_char;
		d = strtol (s.c_str(), &end_char, 0);
		if ((*end_char)!='\0')
			return false;
		return true;
	}

	bool seval (unsigned long int& d, const string& s) {
		char* end_char;
		d = strtoul (s.c_str(), &end_char, 0);
		if ((*end_char)!='\0')
			return false;
		if ((s[0]=='-') && (d!=0))
			return false;
		return true;
	}

	bool seval (int& d, const string& s) {
		long int l;
		bool success = seval (l, s);
		d=l;
		return success;
	}

	bool seval (unsigned int& d, const string& s) {
		unsigned long int l;
		bool success = seval (l, s);
		d=l;
		return success;
	}

	bool seval (double& d, const string& s) {
		char* end_char;
		d = strtod (s.c_str(), &end_char);
		if ((*end_char)!='\0')
			return false;
		return true;
	}

	bool seval (float& d, const string& s) {
		double l;
		bool success = seval (l, s);
		d=l;
		return success;
	}

	bool seval (char& d, const string& s) {
		if (s.length()==0)
			return false;
		d=s[0];
		return (s.length()==1);
	}

	bool seval (bool& d, const string& s) {
		if (s==string("0")) {
			d=false;
			return true;
		}
		if (s==string("1")) {
			d=true;
			return true;
		}
		if (s==string("true")) {
			d=true;
			return true;
		}
		if (s==string("false")) {
			d=false;
			return true;
		}
		return false;
	}

	bool seval (string& d, const string& s) {
		unsigned int p=0;
		d=next_word (s, p, p);
		return true;
	}


	template<class T>
	int get_intern (const string& src, T& dest) {
		bool success = seval (dest, src);
		if (success)
			return 1;
		else
			return -1;
	}

	template<class U>
	int get_intern (const deque<string>& src, vector<U>& dest) {
		bool success = true;
		int n=src.size();
		dest.resize (n);
		for (int i=0; i<n; i++) 
			success &= seval (dest[i], src[i]);
		if (success)
			return n;
		else
			return -n;
	}
}

int ConfigReader::get (const char* key, int& value) const throw (std::bad_alloc) {
	string sv;
	if (!find_first (sv, key)) 
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, unsigned int& value) const throw (std::bad_alloc) {
	string sv;
	if (!find_first (sv, key)) 
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, long int& value) const throw (std::bad_alloc) {
	string sv;
	if (!find_first (sv, key)) 
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, unsigned long int& value) const throw (std::bad_alloc) {
	string sv;
	if (!find_first (sv, key)) 
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, double& value) const throw (std::bad_alloc) {
	string sv;
	if (!find_first (sv, key)) 
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, float& value) const throw (std::bad_alloc) {
	string sv;
	if (!find_first (sv, key)) 
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, char& value) const throw (std::bad_alloc) {
	string sv;
	if (!find_first (sv, key)) 
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, bool& value) const throw (std::bad_alloc) {
	string sv;
	if (!find_first (sv, key)) 
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::string& value) const throw (std::bad_alloc) {
	string sv;
	if (!find_first (sv, key)) 
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::getline (const char* key, std::string& value) const throw (std::bad_alloc) {
	unsigned int p;
	KVPair kv;
	kv.key=key;
	bool found = find_sorted (p, key_value_table, kv);
	if (found)
		value = key_value_table [p].value;
	if (!found && verbosity>=1)
		cerr << "ConfigReader Error: key " << key << " not found\n";
	return found;
}

int ConfigReader::get (const char* key, std::vector<int>& value) const throw (std::bad_alloc) {
	deque<string> sv;
	if (!find_all (sv, key))
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<unsigned int>& value) const throw (std::bad_alloc) {
	deque<string> sv;
	if (!find_all (sv, key))
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<long int>& value) const throw (std::bad_alloc) {
	deque<string> sv;
	if (!find_all (sv, key))
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<unsigned long int>& value) const throw (std::bad_alloc) {
	deque<string> sv;
	if (!find_all (sv, key))
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<double>& value) const throw (std::bad_alloc) {
	deque<string> sv;
	if (!find_all (sv, key))
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<float>& value) const throw (std::bad_alloc) {
	deque<string> sv;
	if (!find_all (sv, key))
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<char>& value) const throw (std::bad_alloc) {
	deque<string> sv;
	if (!find_all (sv, key))
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<string>& value) const throw (std::bad_alloc) {
	deque<string> sv;
	if (!find_all (sv, key))
		return 0;
	return get_intern (sv, value);
}

int ConfigReader::get (const char* key, std::vector<bool>& value) const throw (std::bad_alloc) {
	// Sonderbehandlung, da value<bool> anders gespeichert wird als value<T>
	deque<string> sv;
	if (!find_all (sv, key))
		return 0;
	bool success = true;
	int n=sv.size();
	value.resize (n);
	for (int i=0; i<n; i++) {
		bool b;
		success = success && seval (b, sv[i]);
		value[i]=b;
	}
	if (success)
		return n;
	else
		return -n;
}

bool ConfigReader::write (std::ostream& os, const char* key) const throw () {
	unsigned int p;
	KVPair kv;
	kv.key=key;
	bool found = find_sorted (p, key_value_table, kv);
	if (found) {
		os << key << ' ' << assign_char << ' ' << key_value_table[p].value << '\n';
	}
	if (!found && verbosity>=1)
		cerr << "ConfigReader Error: key " << key << " not found\n";
	return found;
}

void ConfigReader::write_section (std::ostream& os, const char* section) const throw () {
	std::string prefix = std::string(section)+"::";
	os << "[" << section << "]\n";
	for (unsigned int i=0; i<key_value_table.size(); i++) {
		if (key_value_table[i].key.substr(0,prefix.length())==prefix) {
			os << key_value_table[i].key.substr(prefix.length(), key_value_table[i].key.length()) << ' ' << assign_char << ' ' << key_value_table[i].value << '\n';
		}
	}
}



// --------------------- set(.)-Funktionen: -------------------------

void ConfigReader::set (const char* key, const std::string& value) throw (std::bad_alloc) {
	unsigned int p;
	KVPair kv;
	kv.key=key;
	bool found = find_sorted (p, key_value_table, kv);
	if (found)
		key_value_table[p].value=value;   // ersetzen eines bestehenden Wertes
	else {
		kv.value=value;
		key_value_table.insert (key_value_table.begin()+p, kv);  // neuen Wert einfuegen
	}
}

void ConfigReader::set (const char* key, const char* value) throw (std::bad_alloc) {
	string s (value);
	set (key, s);
}

void ConfigReader::set (const char* key, int value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	io << value;
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, unsigned int value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	io << value;
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, long int value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	io << value;
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, unsigned long int value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	io << value;
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, double value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	io << value;
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, float value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	io << value;
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, char value) throw (std::bad_alloc) {
	string sval = " ";
	sval[0]=value;
	set (key, sval);
}

void ConfigReader::set (const char* key, bool value) throw (std::bad_alloc) {
	string sval = ( value ? "true" : "false" );
	set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<int>& value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	for (unsigned int i=0; i<value.size(); i++)
		io << value[i] << ' ';
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<unsigned int>& value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	for (unsigned int i=0; i<value.size(); i++)
		io << value[i] << ' ';
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<long int>& value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	for (unsigned int i=0; i<value.size(); i++)
		io << value[i] << ' ';
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<unsigned long int>& value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	for (unsigned int i=0; i<value.size(); i++)
		io << value[i] << ' ';
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<double>& value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	for (unsigned int i=0; i<value.size(); i++)
		io << value[i] << ' ';
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<float>& value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	for (unsigned int i=0; i<value.size(); i++)
		io << value[i] << ' ';
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<char>& value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	for (unsigned int i=0; i<value.size(); i++)
		io << value[i] << ' ';
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<bool>& value) throw (std::bad_alloc) {
	stringstream io;
	string sval;
	for (unsigned int i=0; i<value.size(); i++)
		io << (value[i] ? "true" : "false" ) << ' ';
	std::getline (io, sval);
	set (key, sval);
}

void ConfigReader::set (const char* key, const std::vector<std::string>& value) throw (std::bad_alloc) {
	string sval = "";
	for (unsigned int i=0; i<value.size(); i++)
		sval += value[i] + ' ';
	set (key, sval);
}

const std::vector<std::string>& ConfigReader::list_of_sources () const throw () {
	return files;
}

void ConfigReader::write (std::ostream& os) const throw () {
	for (unsigned int i=0; i<key_value_table.size(); i++)
		os << key_value_table[i].key << ' ' << assign_char << ' ' << key_value_table[i].value << '\n';
}

ConfigReader::ConfigReader (const ConfigReader& src) throw (std::bad_alloc) :
key_value_table (src.key_value_table),
	verbosity (src.verbosity),
	comment_char (src.comment_char),
	assign_char (src.assign_char),
	directory_stack (src.directory_stack),
	files (src.files),
	home_path (src.home_path),
	shortcuts (src.shortcuts)
{  
	unknown_parameter_counter=0;

}

void ConfigReader::add_command_line_shortcut (const char* sn, const char* ln, bool ha) {
	CmdLineShortcut c;
	c.short_key=sn;
	c.long_key=ln;
	c.has_arg=ha;
	shortcuts.push_back (c);
}

 
void ConfigReader::insert_pair (const std::string& k, const std::string& v) throw (std::bad_alloc) {
	KVPair new_pair;
	new_pair.key=k;
	new_pair.value=v;
	unsigned int pos;
	if (find_sorted (pos, key_value_table, new_pair)) {
		// vorhanden Eintrag ueberschreiben:
		key_value_table[pos]=new_pair;
		if (verbosity>=4)
			cerr << "ConfigReader: replace line " << new_pair.key << " | " << new_pair.value << '\n';
	} else {
		// neuen Eintrag erzeugen
		if (verbosity>=4)
			cerr << "ConfigReader: insert line " << new_pair.key << " | " << new_pair.value << '\n';
		key_value_table.insert (key_value_table.begin()+pos, new_pair);
	}
}

//////////////////////////////////////////////////////////////////////////

bool ConfigReader::delete_config_bak(const char* directoryPath )
{
	DirectoryOperate directory_operate;

	string filename = string( directoryPath );

	std::wstring wfn = Ansi2Unicode(filename);

	directory_operate.DeleteSpecFile( wfn, _T("bak") );

	return true;
}

bool ConfigReader::write_config_file(const char* confile, bool /* =true */)throw (std::bad_alloc)
{
	string fnpath = string(confile);
	string strpath, strfilename;
	extract_path_from_filename_win(strpath,strfilename,fnpath);

	delete_config_bak(strpath.c_str());

	replace_config_file_win(confile);

	return true;
}

const char* ConfigReader::getenv () const throw () 
{
	return home_path.c_str(); 
}

const char* ConfigReader::get_config_path () const throw () 
{ 
 	return conf_name.c_str(); 
}

bool ConfigReader::write_all() throw (std::bad_alloc)
{
	string fnpath = string(get_config_path());
	int pos;
	char b = '/';
	pos = fnpath.find(b); 
	while( pos != -1 )
	{
		fnpath.replace(pos,1,"\\"); 
		pos = fnpath.find(b); 
	} 
	string strpath, strfilename;
	extract_path_from_filename_win(strpath,strfilename,fnpath);
	delete_config_bak(strpath.c_str());

	replace_config_file_win(fnpath.c_str());
 
	return true;
}