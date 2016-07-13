/*
*  console calendar maker ( ccm )
*  console app to generate calendar from year-month input, the output is png image
*  (c) 2014  edi ermawan < edi.ermawan@gmail.com >
*  I added a comment here ( 2016 )
*  i added again
*  change on branch01
*  change on branch01 -2nd time
*  added when in my_branch_02
*/

#include <iostream>
#include <stdio.h>
#include "png.hpp"
#include <pngwriter.h>
#include <sstream>
#include <string>
#include "color.h"
#include "ini.h"

using namespace std;

#define MOONINYEAR 12
#define DAYSINWEEK 7
#define DAY_SPACING "   "
#define DAY_COUNTED_SPACING "  "
#define CALOUT "calendar_out.png"

static char* day_name[DAYSINWEEK] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa" };
static char* month_name[MOONINYEAR] = { "January", "February", "March", "April", "May",
							        "June", "July","August","September","October",
							        "November","December"};
static char* pasaran_name[5] = { " Wage ","Kliwon "," Legi ", "Pahing", " Pon  " };

static const int day_in_month[MOONINYEAR] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

struct s_printer {
	int print_type;
	int year_target;
	int month_target;
	char* background_img;
	int font_size;
	int pasaran_font_size;
	int moon_title_font_size;
	char* font_name;
	int start_x;
	int start_y;
	int pasaran_adjust_x;
	int pasaran_adjust_y;
	int row_space;
	int col_space;
	int month_horizontal_space;
	int month_vertical_space;
	int month_footer_space;
	int default_r;
	int default_g;
	int default_b;
	int img_w;
	int img_h;
};

struct s_holiday {
	int month;
	int day;
	char* info;
};

s_printer printer;
s_holiday holidays[80];
int holiday_index = 0;

bool is_holiday(int month, int day) {
	for (int i = 0; i < holiday_index; i++) {
		if (holidays[i].month == month && holidays[i].day == day) {
			return true;
		}
	}
	return false;
}

bool is_leap_year_1(int year) {
	if ( (year % 4 == 0) ) 
		return true;
	return false;
}

bool is_leap_year_2( int year ) {
	if ( ((year % 4 == 0) && (year % 100)) || (year % 400 == 0) )
		return true;
	return false;
}

bool is_leap_year(int year) {
	if (year > 1581) {
		return is_leap_year_2(year);
	}
	else {
		return is_leap_year_1(year);
	}
}

int get_month_length(bool leap_year, int nth_month) {
	if (nth_month != 1) {
		return day_in_month[nth_month];
	}
	if (leap_year)
		return 29;
	return 28;
}

int get_days_until_month(bool leap_year,int n) {
	int days = 0;
	while (n--)
		days += get_month_length(leap_year, n);
	return days;
}

int get_days_in_year(int y) {
	bool _leap_year = is_leap_year(y);
	return get_days_until_month(_leap_year, MOONINYEAR);
}

int get_days_until_year(int y) {
	int days = 0;
	for (int i = 1; i <= y; i++){
		days += get_days_in_year(i);
	}
	if (y<1582)
		return days;
	else 
		return days-10;
}

void print_text(pngwriter& pw, char* text, int pos_x, int pos_y, s_color color) {
	pw.plot_text(printer.font_name, printer.font_size, pos_x, (printer.img_h - pos_y),
		0, text, color.r, color.g, color.b);
}

void print_text( pngwriter& pw, char* text, int pos_x, int pos_y ) {
	print_text(pw, text, pos_x, pos_y, s_color(printer.default_r, printer.default_g, printer.default_b,1.0f) );
}

s_color define_day_color(int week_index,int day_index, int total_day) {
	s_color color = c_color::Black;
	if (week_index == 0) {
		if (total_day == 7 && day_index == 0)
			color = c_color::Red;
		else if (day_index == total_day - 2)
			color = c_color::Green;
	}
	else if (day_index == 0) {
		color = c_color::Red;
	}
	else if (day_index == 5) {
		color = c_color::Green;
	}
	return color;
}

void print_aday(pngwriter& pw, char* text, int pos_x, int pos_y, s_color color,int p_index) {
	int _nday = atoi(text);
	if (_nday < 10) {
		char ntext[3]; ntext[0] = '0'; ntext[1] = text[0]; ntext[2] = '\0';
		print_text(pw, ntext, pos_x, pos_y, color);
	}
	else {
		print_text(pw, text, pos_x, pos_y, color);
	}
	pw.plot_text(printer.font_name, printer.pasaran_font_size, pos_x + printer.pasaran_adjust_x, 
		(printer.img_h - pos_y) - printer.pasaran_adjust_y,
		0, pasaran_name[p_index], color.r, color.g, color.b);
}

void print_days(char** days, int week_index, int day_total, pngwriter& pw, int pos_x, 
	int pos_y, int* pasaran, int curr_month) {
	int px = pos_x;
	if (week_index == 0) {
		int _space_day = DAYSINWEEK - day_total;
		for (int i = 0; i < _space_day; i++) {
			print_text(pw, "  ", px, pos_y + (week_index * printer.row_space) );
			px += (printer.col_space * 2);
			print_text(pw, DAY_SPACING, px, pos_y + (week_index * printer.row_space) );
			px += (printer.col_space * 2);
		}
	}
	for (int i = 0; i < day_total; i++) {
		s_color color = define_day_color(week_index, i, day_total);
		if( is_holiday(curr_month, atoi(days[i])) ) {
			color = c_color::Red;
		}
		print_aday(pw, days[i], px, pos_y + (week_index * printer.row_space), color, pasaran[i]);
		px += (printer.col_space * 2);
		print_text(pw, DAY_SPACING, px, pos_y + (week_index * printer.row_space));
		px += (printer.col_space * 2);
	}
}

void process_text(pngwriter& pw, int week_index, char* text, int pos_x, int pos_y, int* pasaran, int curr_month) {
	int _day_index  = 0;
	int _day_digit = 0;
	char _day_text[3];
	char* _days_text[DAYSINWEEK];
	while ( *text!='\0' ) {
		if ( *text != ' ' ) {
			_day_text[0] = *text++;
			_day_text[1] = *text++;
			_day_text[2] = '\0';
			_days_text[_day_index] = new char[3];
			memcpy(_days_text[_day_index++], _day_text, 3);
		}
		else {
			*text++;
		}
	}
	print_days(_days_text, week_index, _day_index, pw, pos_x, pos_y, pasaran, curr_month);

	for (int j = 0; j < _day_index; j++) {
		delete _days_text[j];
	}
}

int print_month_header(pngwriter& pw, int year, int month, int px, int py) {
	stringstream _ssmy;
	_ssmy << month_name[month - 1];
	_ssmy << " ";
	_ssmy << year;
	print_text(pw, const_cast<char*>( _ssmy.str().c_str() ), px, py);
	py = py + (printer.row_space);
	for (int i = 0; i < 7; i++) {
		print_text(pw, day_name[i], px, py );
		px += (printer.col_space * 2);
		print_text(pw, DAY_SPACING, px, py );
		px += (printer.col_space * 2);
	}
	return py;
}

void print_month_footer(pngwriter& pw, int month, int px, int py) {
	stringstream _ssfooter;
	int _n = 0;
	for (int i = 0; i < holiday_index; i++) {
		if (holidays[i].month == month) {
			_ssfooter << holidays[i].day;
			_ssfooter << " : ";
			_ssfooter << holidays[i].info;
			_ssfooter << "  ";
			_n++;
			if (_n == 2) {
				_n = 0;
				pw.plot_text(printer.font_name, printer.pasaran_font_size, px, (printer.img_h - py), 0,
					const_cast<char*>(_ssfooter.str().c_str()), c_color::Blue.r, c_color::Blue.g, c_color::Blue.b);
				_ssfooter.str("");
				_ssfooter.clear();
				py += printer.month_footer_space;
			}
		}
	}
	pw.plot_text(printer.font_name, printer.pasaran_font_size, px, (printer.img_h - py), 0,
		const_cast<char*>(_ssfooter.str().c_str()), c_color::Blue.r, c_color::Blue.g, c_color::Blue.b);
}


void print_month(pngwriter& pw, int year, int month, int total_days_prev_year, int ipx, int ipy ) {
	int _year_target = year;
	int _moon_target = month;

	int _printed_month = _moon_target - 1;
	bool _leapYear = is_leap_year(_year_target);
	int _prev_day = get_days_until_month(_leapYear, _printed_month) + total_days_prev_year;
	int _length_day_in_month = day_in_month[_printed_month];
	int _start_d = _prev_day + 1;
	int _end_d = _start_d + _length_day_in_month;

	int _week_index = 0;
	std::stringstream _stream;
	int px = ipx;
	int py = ipy;
	py = print_month_header(pw, _year_target, _moon_target, px, py);
	py = py + (printer.row_space);
	int* _pasaran = new int[7];
	int _pasaran_index = 0;
	for (int i = _start_d; i < _end_d; ++i) {
		int n = i - _prev_day;
		int mod = (i % 7);
		_pasaran[_pasaran_index++] = (i % 5);
		_stream << n << " ";
		if (mod == 1) {
			std::string s;
			s = _stream.str();
			process_text(pw, _week_index, const_cast<char*>(s.c_str()), px, py, _pasaran, _moon_target);
			_stream.str("");
			_stream.clear();
			_week_index++;
			_pasaran_index = 0;
		}
	}
	std::string s = _stream.str();
	process_text(pw, _week_index, const_cast<char*>(s.c_str()), px, py, _pasaran, _moon_target);
	if ( strcmp(s.c_str(), "") == 0 )
		_week_index -= 1; 
	print_month_footer(pw, _moon_target, px, py+((_week_index )*printer.row_space)+printer.pasaran_adjust_y*3 );
	delete[] _pasaran;
}

void print_month(pngwriter& pw ,int year ,int month, int px, int py) {
	int _prev_day = get_days_until_year(year - 1);
	print_month( pw, year, month, _prev_day, px, py );
}

void print_year(pngwriter& pw, int year, int px, int py) {
	int ncol = 4;
	int nrow = 3;
	int nth_month = 1;

	int _prev_day = get_days_until_year(year - 1);
	int _px = px;
	int _py = py;
	for (int i = 0; i < nrow; i++) {
		for (int j = 0; j < ncol; j++) {
			print_month(pw, year, nth_month, _prev_day, _px, _py);
			nth_month++;
			_px += printer.month_horizontal_space;
		}
		_py += printer.month_vertical_space;
		_px = px;
	}
}

void add_holiday(int m, int d, char * info) {
	holidays[holiday_index].day = d;
	holidays[holiday_index].month = m;
	holidays[holiday_index].info = info;
	holiday_index++;
}

static int config_handler(void* user, const char* section, const char* name,
	const char* value)
{
	#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
	#define MATCH2(s, n) strcmp(section, s) == 0
	
	s_printer* _pprinter = (s_printer*)(user);
	
	if (MATCH("setting", "print_type")) {
		_pprinter->print_type = atoi(value);
	}
	else if (MATCH("setting", "year_target")) {
		_pprinter->year_target = atoi(value);
	}
	else if (MATCH("setting", "month_target")) {
		_pprinter->month_target = atoi(value);
	}
	else if (MATCH("setting", "background_img")) {
		_pprinter->background_img = strdup(value);
	}
	else if ( MATCH("setting", "font_size") ) {
		_pprinter->font_size = atoi(value);
	}
	else if (MATCH("setting", "pasaran_font_size")) {
		_pprinter->pasaran_font_size = atoi(value);
	}
	else if (MATCH("setting", "moon_title_font_size")) {
		_pprinter->moon_title_font_size = atoi(value);
	}
	else if (MATCH("setting", "font_name")) {
		_pprinter->font_name = strdup(value);
	}
	else if (MATCH("setting", "start_x")) {
		_pprinter->start_x = atoi(value);
	}
	else if (MATCH("setting", "start_y")) {
		_pprinter->start_y = atoi(value);
	}
	else if (MATCH("setting", "pasaran_adjust_x")) {
		_pprinter->pasaran_adjust_x = atoi(value);
	}
	else if (MATCH("setting", "pasaran_adjust_y")) {
		_pprinter->pasaran_adjust_y = atoi(value);
	}
	else if (MATCH("setting", "row_space")) {
		_pprinter->row_space = atoi(value);
	}
	else if (MATCH("setting", "col_space")) {
		_pprinter->col_space = atoi(value);
	}
	else if (MATCH("setting", "month_horizontal_space")) {
		_pprinter->month_horizontal_space = atoi(value);
	}
	else if (MATCH("setting", "month_vertical_space")) {
		_pprinter->month_vertical_space = atoi(value);
	} 
	else if (MATCH("setting", "month_footer_space")) {
		_pprinter->month_footer_space = atoi(value);
	}
	else if (MATCH("setting", "default_r")) {
		_pprinter->default_r = atoi(value);
	}
	else if (MATCH("setting", "default_g")) {
		_pprinter->default_g = atoi(value);
	}
	else if (MATCH("setting", "default_b")) {
		_pprinter->default_b = atoi(value);
	}
	else if (MATCH("setting", "img_w")) {
		_pprinter->img_w = atoi(value);
	}
	else if (MATCH("setting", "img_h")) {
		_pprinter->img_h = atoi(value);
	}
	// holidays parsing
	else if (MATCH2("January")) {
		add_holiday(1, atoi(name), strdup(value));
	}
	else if (MATCH2("February")) {
		add_holiday(2, atoi(name), strdup(value));
	}
	else if (MATCH2("March")) {
		add_holiday(3, atoi(name), strdup(value));
	}
	else if (MATCH2("April")) {
		add_holiday(4, atoi(name), strdup(value));
	}
	else if (MATCH2("May")) {
		add_holiday(5, atoi(name), strdup(value));
	}
	else if (MATCH2("June")) {
		add_holiday(6, atoi(name), strdup(value));
	}
	else if (MATCH2("July")) {
		add_holiday(7, atoi(name), strdup(value));
	}
	else if (MATCH2("August")) {
		add_holiday(8, atoi(name), strdup(value));
	}
	else if (MATCH2("September")) {
		add_holiday(9, atoi(name), strdup(value));
	}
	else if (MATCH2("October")) {
		add_holiday(10, atoi(name), strdup(value));
	}
	else if (MATCH2("November")) {
		add_holiday(11, atoi(name), strdup(value));
	}
	else if (MATCH2("December")) {
		add_holiday(12, atoi(name), strdup(value));
	}
	return 0;
}

int get_config(char* conf_name) {
	if (ini_parse(conf_name, config_handler, &printer) < 0) {
		printf("can't load file : %s\n", conf_name);
		return 1;
	}
	return 0;
}

int main(int argc, char** argv) {
	printf("console app to generate calendar from year - month input\n");
	printf("the output is png image\n");
	printf("(c) 2014 edi ermawan <edi.ermawan@gmail.com>\n");
	printf("use : ccm.exe configfilename.ini\n");
	if (argc > 1) {
		if ( get_config(argv[1]) ) {
			printf("error parse config file\n");
			return 1;
		}
	}
	else {
		printf("use : ccm.exe configfilename.ini");
		return 1;
	}

	pngwriter* _png;
	if ( strcmp(printer.background_img,"none") )  {
		_png = new pngwriter();
		_png->readfromfile( printer.background_img );
		_png->pngwriter_rename(CALOUT);
		printer.img_w = _png->getwidth();
		printer.img_h = _png->getheight();
	}
	else {
		_png = new pngwriter(printer.img_w, printer.img_h, 1.0, CALOUT);
	}
	if (printer.print_type == 1) {
		print_month(*_png, printer.year_target, printer.month_target, printer.start_x, printer.start_y);
		printf("done ..!");
	}
	else if (printer.print_type == 0) {
		print_year(*_png, printer.year_target, printer.start_x, printer.start_y);
		printf("done ..!");
	}
	_png->setcompressionlevel(6);
	_png->setgamma(0.7);
	_png->close();
	delete _png;

	return 0;
}