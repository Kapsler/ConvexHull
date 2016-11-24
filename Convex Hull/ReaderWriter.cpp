#include "ReaderWriter.h"

ReaderWriter::ReaderWriter(string inputFilename,
	vector<float>& numbers)
{
	readInputFile(inputFilename);
	extractValuesFromContent(numbers);
}

ReaderWriter::~ReaderWriter()
{
}

void ReaderWriter::GenerateInputFile(const string outputFilename, const vector<int>& numbers)
{
	ofstream fileStream(outputFilename);

	if (fileStream)
	{
		fileStream << numbers.size();
		fileStream << endl;

		for(const auto &i : numbers)
		{
			fileStream << i;
			fileStream << endl;
		}
		fileStream.close();
	}
	else
	{
		cerr << "Couldn't write to " << outputFilename << endl;
	}
}

void ReaderWriter::extractValuesFromContent(vector<float>& numbers)
{
	char buffer;
	int numbercount;

	stringstream contentStream(content);

	contentStream >> numbercount; //first int is numbercount
	buffer = contentStream.get(); //first newline

	cout << "Reading " << numbercount << " numbers." << endl;

	numbers.reserve(numbercount);

	for(auto i = 0; i < numbercount; ++i)
	{
		float nextNumber;
		contentStream >> nextNumber; //float1
		numbers.push_back(nextNumber); 
		buffer = contentStream.get(); //comma
		contentStream >> nextNumber;
		numbers.push_back(nextNumber); //float2
		buffer = contentStream.get(); //newline
	}
}


void ReaderWriter::readInputFile(string inputFilename)
{
	stringstream contentStream;
	ifstream fileStream(inputFilename);

	if(fileStream)
	{
		cout << "Reading from file " << inputFilename << endl;

		contentStream << fileStream.rdbuf();
		fileStream.close();

		content = contentStream.str();
	} else
	{
		cerr << "Inputfile " << inputFilename << " not Found!" << endl;
	}

}

