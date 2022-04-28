#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
using namespace std;

class Exception_Error {
	public:
	string description;
	Exception_Error(string cause) {
		description = cause;
	}
};

struct RGBA {
	int r=0, g=0, b=0, a=0;
};

class Color {
public:
	int red=0, green=0, blue=0, alpha=0;
	Color(RGBA item) {
		red = item.r;
		green = item.g;
		blue = item.b;
		alpha = item.a;
	}
};

RGBA Data_Parsing(string data_line, int data_count) {
	bool color_index_format = 1;
	RGBA result;
	try {	
		if (data_line.find(',') != string::npos) color_index_format = 0;
		if (color_index_format)
		{
			bool data_lost = 0;
			if (strlen(data_line.c_str()) < 8) throw Exception_Error("Error during parsing hex line, line too short");
			for (int i = 0; i < strlen(data_line.c_str()); i++) {
				if (!isxdigit(data_line.c_str()[i])) data_lost = 1;
			}
			if (data_lost || (strlen(data_line.c_str())>8)) cout << "Warning with hex parsing in line: " << data_count << " some data could be lost"<<endl;
			result.r = strtoul(data_line.substr(0, 2).c_str(), NULL, 16);
			result.g = strtoul(data_line.substr(2, 2).c_str(), NULL, 16);
			result.b = strtoul(data_line.substr(4, 2).c_str(), NULL, 16);
			result.a = strtoul(data_line.substr(6, 2).c_str(), NULL, 16);
		}
		else {
			int first_comma = data_line.find(',');
			int second_comma = data_line.substr(first_comma+1, string::npos).find(',');
			if (second_comma == string::npos) throw Exception_Error("Error during parsing decimal line, line too short");
			int third_comma = data_line.substr(first_comma+second_comma+2, string::npos).find(',');
			if (third_comma == string::npos) throw Exception_Error("Error during parsing decimal line, line too short");
			result.r = strtoul(data_line.substr(0, first_comma).c_str(), NULL, 10);
			result.g = strtoul(data_line.substr(first_comma+1, second_comma+1).c_str(), NULL, 10);
			result.b = strtoul(data_line.substr(first_comma+second_comma+2, third_comma+1).c_str(), NULL, 10);
			result.a = strtoul(data_line.substr(first_comma+second_comma+third_comma+3, string::npos).c_str(), NULL, 10);
			if (result.r > 255 || result.r < 0 || result.g>255 || result.g < 0 || result.b>255 || result.b < 0 || result.a>255 || result.a < 0) throw Exception_Error("At least one of the values are too large or negative");
		}
		return result;
	}
	catch (Exception_Error Error) {
		cout << Error.description<<endl;
		result.r = result.g = result.b = result.a =-1;
		return result;
	}
	catch (...) {
		cout << "Error during parsing"<<endl;
		result.r = result.g = result.b = result.a = -1;
		return result;
	}
}

double hue_calculating(int red, int green, int blue) {
	try
	{
		double r = red, g = green, b = blue;
		if (r == g && g == b)
		{
			cout << "All RGB values are the same - can't calculate hue"<<endl;
			return 0.0;
		}
		else if (r >=g && g >= b)return 60.0 * (g - b) / (r - b);
		else if (g > r && r>= b)return 60.0 * (2.0 - (r - b) / (g - b));
		else if (g >= b&& b> r)return 60.0 * (2.0 + (b - r) / (g - r));
		else if (b > g&& g> r)return 60.0 * (4.0 - (g - r) / (b - r));
		else if (b > r && r>= g)return 60.0 * (4.0 + (r - g) / (b - g));
		else if (r >= b&& b> g)return 60.0 * (6.0 - (b - g) / (r - g));
	}
	catch (...) {
		cout<<"Error during hue calculation";
	}
}

double saturation_calculating(int red, int green, int blue, double lightness) {
	if (lightness == 1) return 0.0;
	else {
		double r = red / 255.0, g = green / 255.0, b = blue / 255.0,min,max;
		if (r <= g && r <= b) min = r;
		else if (g <= r && g <= b) min = g;
		else min = b;
		if (r >= g && r >= b) max = r;
		else if (g >= r && g >= b) max = g;
		else max = b;
		return (max-min)/(1 - (2 * lightness - 1));
	}
}

double lightness_calculating(int red, int green, int blue) {
	double r = red / 255.0, g = green / 255.0, b = blue / 255.0, min, max;
	if (r <= g && r <= b) min = r;
	else if (g <= r && g <= b) min = g;
	else min = b;
	if (r >= g && r >= b) max = r;
	else if (g >= r && g >= b) max = g;
	else max = b;
	return 0.5 * (min+max);
}

RGBA sum_of_elements(vector<Color> &matrix) {
	RGBA average;
	for (int i = 0; i < matrix.size(); i++)
	{
		average.r += matrix[i].red;
		average.g += matrix[i].green;
		average.b += matrix[i].blue;
		average.a += matrix[i].alpha;
	}
	average.r = average.r / matrix.size();
	average.g = average.g / matrix.size();
	average.b = average.b / matrix.size();
	average.a = average.a / matrix.size();
	return average;
}

RGBA max_value(vector<Color> &matrix) {
	RGBA value = { 0,0,0,0 };
	for (int i = 0; i < matrix.size(); i++)
	{
		if (value.r<matrix[i].red) value.r = matrix[i].red;
		if (value.g < matrix[i].green) value.g = matrix[i].green;
		if (value.b < matrix[i].blue) value.b = matrix[i].blue;
		if (value.a < matrix[i].alpha) value.a = matrix[i].alpha;
	}
	return value;
}

RGBA min_value(vector<Color> &matrix) {
	RGBA value = { 255,255,255,255 };
	for (int i = 0; i < matrix.size(); i++)
	{
		if (value.r > matrix[i].red) value.r = matrix[i].red;
		if (value.g > matrix[i].green) value.g = matrix[i].green;
		if (value.b > matrix[i].blue) value.b = matrix[i].blue;
		if (value.a > matrix[i].alpha) value.a = matrix[i].alpha;
	}
	return value;
}


int main(int argc, char * argv[])
{
	vector<Color> Color_Matrix; 
	int mode = 0;
	try {
		if (argc > 1){
			int argument_parsed = 1;

			if (argc>3)
			{
				if (!strcmp(argv[argument_parsed],"-m")|| !strcmp(argv[argument_parsed], "--m"))
				{
					argument_parsed++;
					if (!strcmp(argv[argument_parsed], "MODE"))
					{
						argument_parsed++;
						if (!strcmp(argv[argument_parsed], "mix")) mode = 0;
						else if (!strcmp(argv[argument_parsed], "lowest")) mode = 1;
						else if (!strcmp(argv[argument_parsed], "highest")) mode = 2;
						else if (!strcmp(argv[argument_parsed], "mix-saturate")) mode = 3;
						else mode = 0;
						argument_parsed++;
					}
				}

			}
			for (int i = argument_parsed; i < argc; i++)
			{
				RGBA answer = (Data_Parsing(argv[argument_parsed], 1));
				if (answer.r != -1 || answer.g != -1 || answer.b != -1 || answer.a != -1)
				{
					Color_Matrix.push_back(answer);
				}
			}


		}
		ifstream file;
		file.open("color.txt");
		if (!file.is_open()) cout << "Error during opening the file"<<endl;
		else {
			string data;
			int line = 0;
			while (getline(file, data)) {
				RGBA answer = (Data_Parsing(data, line++));
				if (answer.r != -1 || answer.g != -1 || answer.b != -1 || answer.a != -1)
				{
					Color_Matrix.push_back(answer);
				}
			};
			
		}
		if (Color_Matrix.size() == 0) throw Exception_Error("Error no color read");
		double hue=0, lightness=0, saturation=0;
		stringstream hex_value;
		RGBA elements;
		switch (mode)
		{
			case 0:
				elements = sum_of_elements(Color_Matrix);
				hue = hue_calculating(elements.r, elements.g, elements.b);
				lightness = lightness_calculating(elements.r, elements.g, elements.b);
				saturation= saturation_calculating(elements.r, elements.g, elements.b,lightness);
				hex_value << hex << elements.r<<elements.g<<elements.b<<elements.a;
				cout <<setprecision(2)<< "New color contains : red = " << elements.r << " green:" << elements.g << " blue:" << elements.b << " alpha:" << elements.a;
				cout <<" hex:"<<hex_value.str()<< setprecision(2) << " hue:" << (int)hue << " lightness:" << lightness << " saturation:" << saturation << endl;
				break;
			case 1:
				elements = min_value(Color_Matrix);
				hue = hue_calculating(elements.r, elements.g, elements.b);
				lightness = lightness_calculating(elements.r, elements.g, elements.b);
				saturation = saturation_calculating(elements.r, elements.g, elements.b, lightness);
				hex_value << hex << elements.r << elements.g << elements.b << elements.a;
				cout << setprecision(2) << "New color contains : red = " << elements.r << " green:" << elements.g << " blue:" << elements.b << " alpha:" << elements.a;
				cout << " hex:" << hex_value.str() << setprecision(2) << " hue:" << (int)hue << " lightness:" << lightness << " saturation:" << saturation << endl;
				break;
			case 2:
				elements = max_value(Color_Matrix);
				hue = hue_calculating(elements.r, elements.g, elements.b);
				lightness = lightness_calculating(elements.r, elements.g, elements.b);
				saturation = saturation_calculating(elements.r, elements.g, elements.b, lightness);
				hex_value << hex << elements.r << elements.g << elements.b << elements.a;
				cout << setprecision(2) << "New color contains : red = " << elements.r << " green:" << elements.g << " blue:" << elements.b << " alpha:" << elements.a;
				cout << " hex:" << hex_value.str() << setprecision(2) << " hue:" << (int)hue << " lightness:" << lightness << " saturation:" << saturation << endl;
				break;
			case 3:
				elements = sum_of_elements(Color_Matrix);
				hue = hue_calculating(Color_Matrix.back().red, Color_Matrix.back().green, Color_Matrix.back().blue);
				lightness = lightness_calculating(elements.r, elements.g, elements.b);
				saturation = saturation_calculating(elements.r, elements.g, elements.b, lightness);
				lightness = lightness_calculating(Color_Matrix.back().red, Color_Matrix.back().green, Color_Matrix.back().blue);
				hex_value << hex << Color_Matrix.back().red << Color_Matrix.back().green << Color_Matrix.back().blue << Color_Matrix.back().alpha;
				cout << setprecision(2) << "New color contains : red = " << Color_Matrix.back().red << " green:" << Color_Matrix.back().green << " blue:" << Color_Matrix.back().blue << " alpha:" << Color_Matrix.back().alpha;
				cout << " hex:" << hex_value.str() << setprecision(2) << " hue:" << (int)hue << " lightness:" << lightness << " saturation:" << saturation << endl;
				break;
		}
		file.close();
		Color_Matrix.clear();
	}
	catch (Exception_Error Error) {
		cout << Error.description<<endl;
	}
	catch(...){
		cout << "Unexpected Error, closing application";
	}
	

}
