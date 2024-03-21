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

struct Field
{
	char field[GlobalConstants::MAX_FIELD_SIZE];
	bool isHeader=false;
};

struct Row
{
	Field fields[GlobalConstants::MAX_FIELDS_ROW];
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
	HtmlTable(Row* rows)
	{
		
	}
	size_t getRowsCount()const;
	size_t getCollsCount()const;
	const Row* getRows () const;
	void setRow(const Row& row, int ind);
	void setRowsCount(size_t count);
	void setCollsCount(size_t count);


	void addRow(const Row& row);
	void printTable();
	Field tdOrThTag(std::ifstream& file, char* targetTag);
	Row trTag(std::ifstream& file);
	void tableTag(std::ifstream& file, HtmlTable& table);
	void readFromFile(const char* fileName, HtmlTable& table);
};



enum class ValidTags
{
	table,
	tr,
	th,
	td
};

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
	this->rowsCount=count;
}

void HtmlTable::setCollsCount(size_t count)
{
	this->collsCount = count;
}

void HtmlTable::addRow(const Row& row)
{
	rows[rowsCount] = row;
	rowsCount++;
}

//ne e okay
char* returnTag(std::ifstream& ifs)
{	
	char buff[8];
	
	while (true) 

	{
		char fst;
		ifs.get(fst);
	
		if (fst == '<')
		{
			
			ifs.getline(buff, 6, '>');
		}

		if (strcmp(buff, "table") == 0 || strcmp(buff, "tr") == 0 || strcmp(buff, "td") == 0 || strcmp(buff, "th") == 0) return buff;
	}
}

Field HtmlTable::tdOrThTag(std::ifstream& file, char* targetTag)
{
	//int localCollsCount = 0;
	Field f{ "" };

	char buff[1024];
	char tag[5];

	while (true)
	{
		file.getline(buff, 1024, '<');
		strcat(f.field, buff);//localCollsCount++;

		file.getline(tag, 5, '>');
		if (strcmp(tag, targetTag) == 0)
		{
			break;
		}
		if (strcmp("th", targetTag) == 0)f.isHeader = true;
		strcat(f.field, tag);
		
	}
	//if (localCollsCount > getCollsCount())setCollsCount(localCollsCount);
	return f;
}

Row HtmlTable::trTag(std::ifstream& file)
{
	char tag[5];
	char buff[1024];
	Row r = {};
	int i = 0;
	char targetTag[4]{ "" };

	while (true)
	{
		
		file.getline(buff, 1024, '<');
		file.getline(tag, 5,'>');
		if (!strcmp(tag, "/tr"))
		{
			break;
		}
		if (!strcmp(tag, "td"))
		{
			strcpy(targetTag, "/td");
		
		}
		if (!strcmp(tag, "th"))
		{
			r.fields[i].isHeader = true;
			strcpy(targetTag, "/th"); 
			
		}
			r.fields[i] = tdOrThTag(file, targetTag);
			i++; 
	}
	if (i > getCollsCount())setCollsCount(i);
	return r;
}

void HtmlTable::tableTag(std::ifstream& file, HtmlTable& table)
{
	char tag[7];
	char buff[1024];
		file.getline(buff, 1024, '<');
		file.getline(tag, 5, '>');
	while (true)
	{
			if (strcmp(tag, "tr") == 0)
			{
				table.addRow(trTag(file));
			}
			
			file.getline(buff, 1024, '<');
			file.getline(tag, 7, '>');
		
		if (strcmp(tag, "/table") == 0)
		{
			break;
		}
	}

}

void HtmlTable::printTable()
{
	for (int i = 0; i < getRowsCount(); i++)
	{
		for (int j = 0; j < getCollsCount(); j++)

		{
			if (rows[i].fields[j].isHeader)
			{
				std::cout << "*"<<rows[i].fields[j].field << "*|";
			}
			else std::cout << rows[i].fields[j].field<< " | ";
		}
		std::cout << std::endl;
	}
	

}

void HtmlTable::readFromFile(const char* fileName, HtmlTable& table)
{
	std::ifstream ifs(fileName);
	char str[8]{ "" };
	strcpy(str, returnTag(ifs));
	if (strcmp(str, "table") == 0)
	{
		tableTag(ifs, table);
	}

	ifs.close();
}

int main()
{
	//std::ifstream ifs("test.txt");
	HtmlTable h;
	
	h.readFromFile("test.txt", h);
	h.printTable();
	

}
