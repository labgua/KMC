/*
  This file is a part of KMC software distributed under GNU GPL 3 licence.
  The homepage of the KMC project is http://sun.aei.polsl.pl/kmc

  This file demonstrates the example usage of kmc_api software. 
  It reads kmer_counter's output and prints kmers to an output file.

  Authors: Sebastian Deorowicz, Agnieszka Debudaj-Grabysz, Marek Kokot

  Version: 3.1.0
  Date   : 2018-05-10

  [Modified]
  This modified version of kmc_dump prints with the line also the 
  sequence identifier, based on the database name previusly generated from
  the program kmc.
  The result is as follows:

  <kmc_database>    <numOccurances>    <K-mer>

  Author: Sergio Guastaferro [labgua] 2018

*/

#include "stdafx.h"
#include <iostream>
#include "../kmc_api/kmc_file.h"
#include "nc_utils.h"
#include <string.h>


void print_info(void);


//----------------------------------------------------------------------------------
// Check if --help or --version was used
bool help_or_version(int argc, char** argv)
{
	const std::string version = "--version";
	const std::string help = "--help";
	for (int i = 1; i < argc; ++i)
	{
		if (argv[i] == version || argv[i] == help)
			return true;
	}
	return false;
}

int _tmain(int argc, char* argv[])
{
	if (argc == 1 || help_or_version(argc, argv))
	{
		print_info();
		return 0;
	}

	CKMCFile kmer_data_base;
	int32 i;
	uint32 min_count_to_set = 0;
	uint32 max_count_to_set = 0;
	std::string input_file_name;
	std::string output_file_name;
	char* simple_name = NULL; // Label added to the line

	FILE * out_file;
	//------------------------------------------------------------
	// Parse input parameters
	//------------------------------------------------------------
	if(argc < 3)
	{
		print_info();
		return EXIT_FAILURE;
	}

	for(i = 1; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{	
			if(strncmp(argv[i], "-ci", 3) == 0)
				min_count_to_set = atoi(&argv[i][3]);
			else if(strncmp(argv[i], "-cx", 3) == 0)
					max_count_to_set = atoi(&argv[i][3]);
		}
		else
			break;
	}

	if(argc - i < 2)
	{ 
		print_info();
		return EXIT_FAILURE;
	}

	input_file_name = std::string(argv[i++]);
	output_file_name = std::string(argv[i]);

	printf("|%s|\n", input_file_name.c_str());

	char* input_file_name_clone = (char*) malloc( input_file_name.length() );
	input_file_name.copy(input_file_name_clone, input_file_name.length(), 0);

	//setting simple_name
	char *token = strtok( input_file_name_clone, "/");
	while (token != NULL)
    {
        simple_name = token;
        token = strtok(NULL, "/");
    }

    printf("|%s|\n", simple_name);


	if((out_file = fopen (output_file_name.c_str(),"wb")) == NULL)
	{
		print_info();
		return EXIT_FAILURE;
	}

	setvbuf(out_file, NULL ,_IOFBF, 1 << 24);

	//------------------------------------------------------------------------------
	// Open kmer database for listing and print kmers within min_count and max_count
	//------------------------------------------------------------------------------

	if (!kmer_data_base.OpenForListing(input_file_name))
	{
		print_info();
		return EXIT_FAILURE ;
	}
	else
	{
		uint32 _kmer_length;
		uint32 _mode;
		uint32 _counter_size;
		uint32 _lut_prefix_length;
		uint32 _signature_len;
		uint32 _min_count;
		uint64 _max_count;
		uint64 _total_kmers;

		kmer_data_base.Info(_kmer_length, _mode, _counter_size, _lut_prefix_length, _signature_len, _min_count, _max_count, _total_kmers);

		
		//std::string str;
		char str[1024];
		uint32 counter_len;
		
		CKmerAPI kmer_object(_kmer_length);
		
		if(min_count_to_set)
		if (!(kmer_data_base.SetMinCount(min_count_to_set)))
				return EXIT_FAILURE;
		if(max_count_to_set)
		if (!(kmer_data_base.SetMaxCount(max_count_to_set)))
				return EXIT_FAILURE;	

		if (_mode) //quake compatible mode
		{
			float counter;
			while (kmer_data_base.ReadNextKmer(kmer_object, counter))
			{
				kmer_object.to_string(str);
				str[_kmer_length] = '\t';				
				counter_len = CNumericConversions::Double2PChar(counter, 6, (uchar*)str + _kmer_length + 1);				
				str[_kmer_length + 1 + counter_len] = '\n';
				fwrite(str, 1, _kmer_length + counter_len + 2, out_file);			
			}
		}
		else
		{
			uint64 counter;
			while (kmer_data_base.ReadNextKmer(kmer_object, counter))
			{
				
				// stampa nella linea anche il nome del file
				strcpy(str, simple_name);
				int len_simple_name = strlen(simple_name);
				str[len_simple_name] = '\t';

				kmer_object.to_string( str + len_simple_name + 1 );
				str[len_simple_name + 1 + _kmer_length] = '\t';
				counter_len = CNumericConversions::Int2PChar(counter, (uchar*)str + len_simple_name + 1 + _kmer_length + 1);
				str[len_simple_name + 1 + _kmer_length + 1 + counter_len] = '\n';
				fwrite(str, 1, len_simple_name + 1 + _kmer_length + 1 + counter_len + 1, out_file);
				
				/*
				Non piu utile...
				counter_len = CNumericConversions::Int2PChar(counter, (uchar*)str);
				str[counter_len] = '\t';
				kmer_object.to_string(str + counter_len + 1);
				str[_kmer_length + 1 + counter_len] = '\n';
				fwrite(str, 1, _kmer_length + counter_len + 2, out_file);
				*/
			}
		}
		
	
		fclose(out_file);
		kmer_data_base.Close();
	}

	return EXIT_SUCCESS; 
}
// -------------------------------------------------------------------------
// Print execution options 
// -------------------------------------------------------------------------
void print_info(void)
{
	std::cout << "KMC dump INDEXED ver. " << KMC_VER << " (" << KMC_DATE << ")\n"
			  << "\nUsage:\nkmc_dump [options] <kmc_database> <output_file>\n"
			  << "Parameters:\n"
			  << "<kmc_database> - kmer_counter's output\n"
			  << "Options:\n"
			  << "-ci<value> - exclude k-mers occurring less than <value> times\n"
			  << "-cx<value> - exclude k-mers occurring more of than <value> times\n";
};

// ***** EOF
