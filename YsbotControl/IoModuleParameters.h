#ifndef _io_module_parameters_h
#define _io_module_parameters_h
 
// #define  MCC_RIO      (0)
// #define  MCC_LIO      (1)
// #define  MCC_GPIO     (2)

struct  IoModuleParameters
{ 
	int x_point_num; // bit
	int y_point_num;
	int mx_relay_num;
	int my_relay_num;
	int d_register_num; // word

	int x_fst_address; // 
	int y_fst_address;
	int mx_fst_address; // M register address
	int my_fst_address;
	int d_fst_address; // Data register address

	int y_word_size;   // word size for Y(16)
	int my_word_size;
	int x_word_size;   // word size for X(16)
	int mx_word_size;
	
	int local_input_size;

	int max_read_word;
	long int scan_cycle_time;

	bool     is_connected;

	IoModuleParameters () : x_point_num(0),y_point_num(0), mx_relay_num(0), my_relay_num(0), d_register_num(0),is_connected(false),
		x_fst_address(7100),y_fst_address(7200),mx_fst_address(7110),my_fst_address(7210),d_fst_address(7300),y_word_size(0),my_word_size(0),
	x_word_size(0),mx_word_size(0),max_read_word(10),scan_cycle_time(100),local_input_size(4) {;}
	 IoModuleParameters (const  IoModuleParameters& fp) { operator= (fp); }
	const  IoModuleParameters& operator= (const  IoModuleParameters& fp) 
	{
		x_point_num    = fp.x_point_num;
		y_point_num    = fp.y_point_num;
		mx_relay_num   = fp.mx_relay_num;
		my_relay_num   = fp.my_relay_num;
		d_register_num = fp.d_register_num;

		x_fst_address  = fp.x_fst_address;
		y_fst_address  = fp.y_fst_address;
		mx_fst_address = fp.mx_fst_address;
		my_fst_address = fp.my_fst_address;
		d_fst_address  = fp.d_fst_address;

		y_word_size    = fp.y_word_size; 
		my_word_size   = fp.my_word_size;
		x_word_size    = fp.x_word_size; 
		mx_word_size   = fp.mx_word_size;
		
		max_read_word   = fp.max_read_word;
		scan_cycle_time = fp.scan_cycle_time;
		is_connected    = fp.is_connected;

		local_input_size = fp.local_input_size;

		return *this;
	}
};


#endif

