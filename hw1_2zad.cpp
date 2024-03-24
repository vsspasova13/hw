#include <iostream>
#include <fstream>
#pragma warning(disable: 4996)

namespace GlobalConstants
{
	constexpr int MAX_NAME_SIZE = 64;
	constexpr int MAX_CONTENT_SIZE = 256;
	constexpr int MAX_SONGS_COUNT = 30;
	constexpr int GENRES_COUNT = 5;
}

//void makeBitOne(char& n)
//{
//	int mask = 1;
//	n |= mask;
//}

struct Duration
{
	int hours = 0;
	int minutes = 0;
	int seconds = 0;
};

enum class Genre :char
{
	Pop = 1 << 0,
	Rock = 1 << 1,
	Jazz = 1 << 2,
	Electronic = 1 << 3,
	HipHop = 1 << 4,
	undefined = 0
};

void OneGenrePrint(Genre g)
{
	switch (g)
	{
	case Genre::Rock:std::cout << "Rock"; break;
	case Genre::Pop:std::cout << "Pop"; break;
	case Genre::Electronic:std::cout << "Electronic"; break;
	case Genre::HipHop:std::cout << "Hip-Hop"; break;
	case Genre::Jazz:std::cout << "Jazz"; break;
	default: std::cout << "undefined";
		break;
	}
}

Genre chooseGenre(char ch)
{
	switch (ch)
	{
	case 'r':return Genre::Rock; break;
	case 'p':return Genre::Pop; break;
	case 'e':return Genre::Electronic; break;
	case 'h':return Genre::HipHop; break;
	case 'j':return Genre::Jazz; break;
	default: return Genre::undefined;
		break;
	}
}

struct Content
{
	char cnt[256];
	size_t size;

	void readFromBinaryFile(const char* fileName)
	{
		std::ifstream ifs(fileName, std::ios::binary);
		ifs.read(cnt, sizeof(cnt));
	}
};

class Song
{
	char name[GlobalConstants::MAX_NAME_SIZE]{};
	Duration duration{ 0,0,0 };
	char genre = ' ';
	char content[GlobalConstants::MAX_CONTENT_SIZE]{};

public:
	Song()
	{

	}


	Song(const char* name, const int h, const int min, const int sec, const char* genre, const char* content)
	{

		setName(name);
		setDuration(h, min, sec);
		setGenre(genre);
		setContent(content);

	}

	void setName(const char* name)
	{
		if (strlen(name) <= GlobalConstants::MAX_NAME_SIZE) { strcpy(this->name, name); }
	}

	void setDuration(int h, int min, int sec)
	{
		if (h >= 0 && h <= 23)this->duration.hours = h;
		if (min >= 0 && min <= 60)this->duration.minutes = min;
		if (sec >= 0 && sec <= 59)this->duration.seconds = sec;
	}
	void setGenre(const char* genre)
	{
		if (!genre)return;

		while (*genre)
		{
			this->genre |= (char)chooseGenre(*(genre));;
			genre++;
		}
	}

	void setContent(const char* cnt)
	{
		for (int i = 0; i < GlobalConstants::MAX_CONTENT_SIZE; i++)
		{
			this->content[i] = cnt[i];
		}
	}

	const char* getName() const
	{
		return this->name;
	}

	const Duration getDuration() const
	{
		return this->duration;
	}

	const char* getContent() const
	{
		return this->content;
	}

	const char getGenre() const
	{
		return this->genre;
	}

	void printDuration(Duration d)
	{
		if (d.hours < 10)std::cout << 0;
		std::cout << d.hours << ":";
		if (d.minutes < 10)std::cout << 0;
		std::cout << d.minutes << ":";
		if (d.seconds < 10)std::cout << 0;
		std::cout << d.seconds;
	}

	void printGenres()
	{
		bool first = true;
		for (int i = 0; i < GlobalConstants::GENRES_COUNT; i++)
		{
			Genre currG = (Genre)(1 << i);
			if ((this->genre & (char)currG) == (char)currG)
			{
				if (!first)
				{
					std::cout << "&";
				}
				OneGenrePrint(currG);
				first = false;
			}
		}
	}

	void printSong()
	{

		std::cout << getName() << ", ";
		printDuration(this->duration);
		std::cout << ", ";
		printGenres();
		std::cout << std::endl;
	}

	//void setContentByIndex(char ch, int i)
	//{
	//	this->content[i] = ch;
	//}

	void mix(const Song& other, size_t size)
	{
		for (int i = 0; i < size; ++i)
		{
			content[i] ^= other.content[i];
		}
	}

	void setContentIndex(char ch, int ind)
	{
		this->content[ind] = ch;
	}

	char mixK(int i)
	{
		int mask = 1 << i;
		char ch = mask | this->content[i];
		//this->content[i] = ch;
		return ch;
	}
};


class Playlist
{
	Song songs[GlobalConstants::MAX_SONGS_COUNT];
	size_t songsCount = 0;

public:
	void add(const char* name, const int h, const int min, const int sec, const char* genre, const char* fileName)
	{
		std::ifstream ifs(fileName, std::ios::binary | std::ios::in);
		char content[GlobalConstants::MAX_CONTENT_SIZE];
		ifs.read((char*)&content, sizeof(content) * GlobalConstants::MAX_CONTENT_SIZE);

		Song s{ name,h,min,sec, genre,content };
		songs[songsCount++] = s;
		ifs.close();
	}

	void print()
	{
		for (int i = 0; i < songsCount; i++)
		{
			songs[i].printSong();
		}
	}

	void find(const char* nameSearch)
	{
		for (int i = 0; i < songsCount; i++)
		{
			if (strcmp(songs[i].getName(), nameSearch) == 0)
			{
				songs[i].printSong();
				return;
			}
		}
		std::cout << "No results" << std::endl;
	}

	void findGenre(const char ch)
	{
		Genre gr = chooseGenre(ch);
		for (int i = 0; i < songsCount; i++)
		{
			if (((int)songs[i].getGenre() & (int)gr) == (int)gr)songs[i].printSong();
		}
	}

	void sortByDuration()
	{
		for (int i = 0; i < songsCount; i++)
		{
			int minInd = i;
			int durr1 = songs[i].getDuration().hours * 3600 + songs[i].getDuration().minutes * 60 + songs[i].getDuration().seconds;
			for (int j = i + 1; j < songsCount; j++)
			{
				int durr2 = songs[j].getDuration().hours * 3600 + songs[j].getDuration().minutes * 60 + songs[j].getDuration().seconds;
				if (durr1 > durr2) minInd = j;
			}
			if (minInd != i)
				std::swap(songs[i], songs[minInd]);
		}
	}

	void sortByName()
	{
		for (int i = 0; i < songsCount; i++)
		{
			int minInd = i;
			for (int j = i + 1; j < songsCount; j++)
			{
				if (strcmp(songs[minInd].getName(), songs[j].getName()) > 0) minInd = j;
			}
			if (minInd != i)
				std::swap(songs[i], songs[minInd]);
		}
	}

	int findIndOfSongByName(const char* name)
	{
		for (int i = 0; i < songsCount; i++)
		{
			if (strcmp(songs[i].getName(), name) == 0)
			{
				return i;
			}
		}
		return -1;
	}

	//int findSizeOfContent(const char* str)
	//{
	//	size_t len = strlen(str);
	//	return len;
	//}

	void mix(const char* song1, const char* song2)
	{
		int indFirst = findIndOfSongByName(song1);
		int indSec = findIndOfSongByName(song2);
		size_t size1 = strlen(songs[indFirst].getContent());

		songs[indFirst].mix(songs[indSec], size1);

	}

	void mixSongK(Song& song, int k)
	{
		size_t size1 = strlen(song.getContent());
		for (size_t i = 0; i < size1; i++)
		{
			char ch = song.getContent()[i];
			for (int j = k-1; j <  8; j += k)
			{
				ch = ch | (1 << j);
				song.setContentIndex(ch, i);

			}
		}
	}

	void mixK(const char* song, int k)
	{
		int indFirst = findIndOfSongByName(song);
		mixSongK(songs[indFirst], k);
	}

	void save(const char* song, const char* fileName)
	{
		std::ofstream ofs(fileName, std::ios::binary);
		int indFirst = findIndOfSongByName(song);
		size_t size1 = strlen(songs[indFirst].getContent());

		ofs.write((const char*)songs[indFirst].getContent(), size1);

		ofs.close();
	}
};

int main()
{


	/*std::ofstream ofs("song2.dat", std::ios::binary);
	char pp[] = "Ua";
	ofs.write((char*)&pp, sizeof(pp));
	ofs.close();*/

	std::ofstream fs("song1.dat", std::ios::binary);
	char pp[] = "V";
	fs.write((char*)&pp, sizeof(pp));
	fs.close();


	Playlist p;
	p.add("Song 2", 0, 1, 55, "rp", "song2.dat");
	p.add("Song 1", 0, 1, 5, "p", "song1.dat");
	p.print();
	std::cout << "sort:" << std::endl;
	p.sortByName();
	p.print();
	std::cout << "find:" << std::endl;
	p.find("Song 1");
	std::cout << "findGP:" << std::endl;
	p.findGenre('p');
	std::cout << "findGR:" << std::endl;
	p.findGenre('r');
	p.sortByDuration();
	p.print();

	p.mixK("Song 1", 3);
	p.save("Song 1", "song3.dat");


	//p.mix("Song 1", "Song 2");
	//p.save("Song 1", "song3.dat");

	//contenta
}



