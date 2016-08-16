#include "snp.h"

SNP::SNP(const std::string rs_id, const std::string chr, const size_t loc, const std::string ref_allele, const std::string alt_allele, const double statistic, const double p_value):m_ref_allele(ref_allele), m_alt_allele(alt_allele), m_rs_id(rs_id), m_chr(chr), m_loc(loc), m_stat(statistic), m_p_value(p_value) {}

SNP::SNP()
{
    //ctor
}

SNP::~SNP()
{
    //dtor
}


void SNP::read_snp(const std::string c_input_file, boost::ptr_vector<SNP> &snp_list, const Commander commander){
    std::ifstream input;
    input.open(c_input_file.c_str());
    if(!input.is_open()){
        std::string error_message = "Cannot open " + c_input_file + " to read";
        throw std::runtime_error(error_message);
    }
    std::string line;
    while(std::getline(input, line)){
        usefulTools::trim(line);
        std::vector<string> token = usefulTools::split(line, "\t ");

    }
    input.close();
}