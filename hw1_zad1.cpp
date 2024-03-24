#include <iostream>
#include <fstream>
#pragma warning(disable : 4996)

namespace GlobalConstants
{
	constexpr int MAX_FIELD_SIZE = 50;
	constexpr int MAX_FIELDS_ROW = 30;
	constexpr int MAX_ROWS_TABLE = 300;
}

bool isSlash(char ch)
{
	return ch == '/';
}

bool isPrefix(const char* pattern, const char* text)
{
	while (*text != '\0' && *pattern != '\0')
	{
		if (*text != *pattern)
			return false;
		text++;
		pattern++;
	}
	return *pattern == '\0';
}

int searchInText(const char* text, const char* pattern)
{
	size_t textLen = strlen(text);
	size_t patternLen = strlen(pattern);
	int ind = 0;
	while (patternLen <= textLen)
	{
		if (isPrefix(pattern, text))
			return ind;
		text++; ind++;
		textLen--;
	}
	return false;
}


struct Field
{
	char field[GlobalConstants::MAX_FIELD_SIZE];
	bool isHeader = false;

	bool getHeader()
	{
		return this->isHeader;
	}
};

struct Row
{
	Field fields[GlobalConstants::MAX_FIELDS_ROW];
	size_t numberOfFields = 0;
	void setH(int i)
	{
		fields[i].isHeader = true;
	}
	void setRow(const Field* f)
	{
		for (int i = 0; i < numberOfFields; i++)
		{
			this->fields[i] = f[i];
		}
	}
};

class HtmlTable
{
private:
	size_t rowsCount = 0;
	size_t collsCount = 0;
	Row rows[GlobalConstants::MAX_ROWS_TABLE];

public:
	HtmlTable()
	{

	}
	HtmlTable(size_t collsCount, size_t rowsCount,const Row* rows)
	{
		setCollsCount(collsCount);
		setRowsCount(rowsCount);
		setRowsArray(rows, rowsCount);
	}
	size_t getRowsCount()const;
	size_t getCollsCount()const;
	const Row* getRows() const;
	void setRow(const Row& row, int ind);
	void setRowsArray(const Row* rows, size_t count);
	void setRowsCount(size_t count);
	void setCollsCount(size_t count);
	void setFieldHeader(bool isH, int ind, int j);
	void setField(const char* value);

	void addRow(const Row& row, int ind);
	void adddRow(const Row& row);
	void printTable();
	Field tdOrThTag(std::ifstream& file, char* targetTag);
	Row trTag(std::ifstream& file, int& i, int& j);
	void tableTag(std::ifstream& file, HtmlTable& table);
	void readFromFile(const char* fileName, HtmlTable& table);
	int maxLength(int colNum);
	void add(int ind, Row r);
	void remove(int ind);
	void edit(int rowInd, int collInd, const char* newValue);
	void saveToFile(const char* fileName);
};

void HtmlTable::setRowsArray(const Row* rows, size_t count)
{
	for (int i = 0; i < count; i++)
	{
		this->rows[i] = rows[i];
	}
}

size_t HtmlTable::getRowsCount()const
{
	return this->rowsCount;
}

size_t HtmlTable::getCollsCount()const
{
	return this->collsCount;
}

const Row* HtmlTable::getRows()const
{
	return rows;
}

void HtmlTable::setRow(const Row& row, int ind)
{
	rows[ind] = row;
}

void HtmlTable::setRowsCount(size_t count)
{
	this->rowsCount = count;
}

void HtmlTable::setCollsCount(size_t count)
{
	this->collsCount = count;
}

void HtmlTable::addRow(const Row& row, int ind)
{
	rows[ind] = row;
	rowsCount++;
}
void HtmlTable::adddRow(const Row& row)
{
	rows[rowsCount] = row;
	rowsCount++;
}

void HtmlTable::setFieldHeader(bool isH, int ind, int j)
{
	rows[ind].fields[j].isHeader = isH;
}

char* returnTag(std::ifstream& ifs)
{
	char buff[9]; buff[8] = '\0';
	char fst; ifs.get(fst);
	while (true)
	{

		char sec;
		ifs.get(sec);

		if (fst == '<')
		{
			if (sec == 't')
			{
				ifs.getline(buff, 6, '>');
			}
			else continue;
		}

		if (strcmp(buff, "able") == 0 || strcmp(buff, "r") == 0 || strcmp(buff, "d") == 0 || strcmp(buff, "h") == 0) return buff;
	}
}

Field HtmlTable::tdOrThTag(std::ifstream& file, char* targetTag)
{
	Field f{ "" };

	char buff[1024];
	if (strcmp("th", targetTag) == 0) f.isHeader=true;
	char tag[7]; tag[6]='\0';
	file.ignore();
	file.getline(buff, 1024, '<');
	strcat(f.field, buff);

	while (true)
	{
		char ch;
		file.get(ch);
		/*if (strcmp("th", targetTag) == 0)
		{
			size_t len = strlen(f.field);
			f.field[len] = '*';
		}*/
		if (ch == 't')
		{
			continue;
		}
		
		else if (ch == '/')
		{
			file.getline(tag, 5, '>');
			if (strcmp(tag, targetTag) == 0)
			{
				break;
			}
		}
		else{
			size_t len = strlen(f.field);
			f.field[len] = ch;
		continue;
		}

	}
	
	
	return f;
}

Row HtmlTable::trTag(std::ifstream& file, int& rowInd, int& collInd)
{
	char tag[5];
	char buff[1024];
	Row r = {};
	int i = 0;
	char targetTag[4]{ "" }; collInd=0;

	while (true)
	{
		file.getline(buff, 1024, '<');
		
		char ch; file.get(ch);
		if (ch == 't')
		{
			file.get(ch);

			if (ch == 'd')
			{
				strcpy(targetTag, "td");
				collInd++;
			}
			else if (ch == 'h')
			{	
				//r.setH(collInd);
				//r.fields[collInd++].isHeader = true;
				setFieldHeader(true, rowInd, collInd); collInd++;
				strcpy(targetTag, "th");
			}
			else continue;

			r.fields[i] = tdOrThTag(file, targetTag);

		}
		else if (ch == '/') {  rowInd++; break;}
		else file.ignore();
		i++;
	}
	r.numberOfFields = i;
	if (i > getCollsCount())setCollsCount(i);
	return r;
}

void HtmlTable::tableTag(std::ifstream& file, HtmlTable& table)
{
	char tag[7];
	char buff[1024];
	int rollInd = 0, collInd = 0;

	file.getline(buff, 1024, '<');
	while (true)
	{
		char ch; file.get(ch);
		if (ch == 't')
		{
			file.getline(tag, 7, '>');
			if (strcmp(tag, "r") == 0)
			{
				table.adddRow(trTag(file,rollInd, collInd));
			}
			file.getline(buff, 1024, '<');

		}
		else if (ch == '/')break;
	}
}

int HtmlTable::maxLength(int collNum)
{
	int maxLength = 0; bool isH = false;
	for (int i = 0; i < getRowsCount(); i++)
	{
		int len = strlen(rows[i].fields[collNum].field);

		if (len > maxLength) maxLength = len;
		if (rows[i].fields[collNum].isHeader)isH = true;
	}
	if (isH)maxLength += 2;
	return maxLength;
}

void printIntervals(int len1, int maxLength)
{
	int dif = maxLength - len1;

	for (int i = 0; i < dif; i++)
	{
		std::cout << " ";
	}
}

void HtmlTable::printTable()
{
	for (int i = 0; i < getRowsCount(); i++)
	{
		for (int j = 0; j < getCollsCount(); j++)
		{
			int maxLen = maxLength(j);
			int len = strlen(rows[i].fields[j].field);

			if (rows[i].fields[j].isHeader == true)
			{
				len += 2; 
				std::cout << "|*" << rows[i].fields[j].field;
				printIntervals(len, maxLen); 
				std::cout << "*";
			}

			else {

				std::cout << "|" << rows[i].fields[j].field;
				printIntervals(len, maxLen);
			}
		}
		std::cout << "|" << std::endl;
	}

}
void HtmlTable::readFromFile(const char* fileName, HtmlTable& table)
{
	std::ifstream ifs(fileName);
	char str[8]{ "" };
	strcpy(str, returnTag(ifs));

	if (strcmp(str, "able") == 0)
	{
		tableTag(ifs, table);
	}
	else ifs.ignore();

	ifs.close();
}

void HtmlTable::add(int rowNum, Row row)
{
	if (rowNum > getRowsCount()) { setRowsCount(rowNum); addRow(row, rowNum); }
	else
	{
		setRowsCount(getRowsCount() + 1);
		for (int i = getRowsCount() - 1; i > rowNum - 1; i--)
		{
			std::swap(rows[i], rows[i - 1]);
		}
		setRow(row, rowNum - 1);
	}
}

void HtmlTable::remove(int ind)
{
	for (int i = ind - 1; i < getRowsCount() - 1; i++)
	{
		std::swap(rows[i], rows[i + 1]);
	}
	setRowsCount(getRowsCount() - 1);
}

void HtmlTable::edit(int rowInd, int collInd, const char* newValue)
{
	strcpy(rows[rowInd - 1].fields[collInd - 1].field, newValue);
}

void HtmlTable::saveToFile(const char* fileName)
{
	std::ofstream ofs(fileName);
	bool isH = false;
	ofs << "<table>" << std::endl;
	
	for (int i = 0; i < rowsCount; i++)
	{
		ofs << "<tr>" << std::endl;
		for (int j = 0; j < collsCount; j++)
		{
			if (rows[i].fields[j].isHeader==true)isH = true;
			if (isH)
			{
				ofs << "<th>"<<rows[i].fields[j].field<<"</th>"<< std::endl;
			}
			else
			{
				ofs << "<td>"<<rows[i].fields[j].field<<"</td>"<< std::endl;
			}
			isH = false;
		}
		ofs << "</tr>" << std::endl;
	}
	ofs << "</table>" << std::endl;
}


int main()
{
	HtmlTable h;

	h.readFromFile("test.txt", h);
	h.printTable();
	std::cout << std::endl;
	
	int num = 0;
	std::cin >> num;

	Row r= { "kxkd","aa","Spasov" };
	h.add(num, r);
	h.printTable();
	std::cout << std::endl;
	h.remove(1);
	h.printTable();
	std::cout << std::endl;
	h.edit(2, 1, "vili");
	h.printTable();
	
	h.saveToFile("save.txt");

	//kodirovka
	
	//validacii
}