// This file is part of PRSice2.0, copyright (C) 2016-2017
// Shing Wan Choi, Jack Euesden, Cathryn M. Lewis, Paul F. O’Reilly
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "commander.hpp"
#include "genotype.hpp"
#include "genotypefactory.hpp"
#include "prsice.hpp"
#include "region.hpp"
#include "reporter.hpp"
int main(int argc, char* argv[])
{
    Reporter reporter;
    Commander commander;
    try
    {
        if (!commander.init(argc, argv, reporter))
            return 0; // only require the usage information
    }
    catch (const std::runtime_error& error)
    {
        return -1; // all error messages should have printed
    }
    bool verbose = true;
    // this allow us to generate the appropriate object (i.e. binaryplink /
    // binarygen)
    GenomeFactory factory;
    Genotype* target_file;
    try
    {
        target_file = factory.createGenotype(
            commander.target_name(), commander.target_type(),
            commander.thread(), commander.ignore_fid(), commander.nonfounders(),
            commander.keep_ambig(), reporter, commander);
        target_file->load_samples(commander.keep_sample_file(),
                                  commander.remove_sample_file(), reporter);
        target_file->load_snps(
            commander.extract_file(), commander.exclude_file(),
            commander.geno(), commander.maf(), commander.info(),
            commander.hard_threshold(), commander.hard_coded(), reporter);
    }
    catch (const std::invalid_argument& ia)
    {
        reporter.report(ia.what());
        return -1;
    }
    catch (const std::runtime_error& error)
    {
        reporter.report(error.what());
        return -1;
    }
    bool used_ld = false;
    Genotype* ld_file = nullptr;
    if (!commander.ld_prefix().empty()
        && commander.ld_prefix().compare(commander.target_name()) != 0)
    {
        used_ld = true;
        ld_file =
            factory.createGenotype(commander, commander.ld_prefix(),
                                   commander.ld_type(), verbose, reporter);
    }

    Region region = Region(commander.feature(), target_file->get_chr_order());
    try
    {
        region.run(commander.gtf(), commander.msigdb(), commander.bed(),
                   commander.out());
    }
    catch (const std::runtime_error& error)
    {
        reporter.report(error.what());
        return -1;
    }

    // Might want to generate a log file?
    region.info();

    bool perform_prslice = commander.perform_prslice();

    // Need to handle paths in the name
    std::string base_name = misc::remove_extension<std::string>(
        misc::base_name<std::string>(commander.base_name()));
    std::string message = "Start processing " + base_name + "\n";
    message.append("==============================\n");
    reporter.report(message);
    try
    {
        target_file->set_info(commander);
        target_file->read_base(commander, region, reporter);
        // we no longer need the region boundaries
        // as we don't allow multiple base file input
        region.clean();
        std::string region_out_name = commander.out() + ".region";
        // output the number of SNPs observed in each sets
        region.print_file(region_out_name);
        // perform clumping (Main problem with memory here)
        if (!commander.no_clump()) {
            target_file->efficient_clumping(
                (ld_file == nullptr) ? *target_file : *ld_file, reporter);
        }
        // initialize PRSice class
        PRSice prsice = PRSice(base_name, commander, region.size() > 1,
                               target_file->num_sample(), reporter);
        // check the phenotype input columns
        prsice.pheno_check(commander, reporter);
        size_t num_pheno = prsice.num_phenotype();
        if (!perform_prslice) {
            if (!target_file->prepare_prsice()) {
                // check if we can successfully sort the SNP vector by the
                // category as required by PRSice
                return -1;
            }
            for (size_t i_pheno = 0; i_pheno < num_pheno; ++i_pheno) {
                // initialize the phenotype & independent variable matrix
                prsice.init_matrix(commander, i_pheno, *target_file, reporter);
                // go through each region separately
                // this should reduce the memory usage
                if (region.size() > 1) {
                    fprintf(stderr, "\rProcessing %03.2f%% of sets", 0.0);
                }
                for (size_t i_region = 0; i_region < region.size(); ++i_region)
                {
                    prsice.run_prsice(commander, region.get_name(i_region),
                                      i_pheno, i_region, *target_file);
                    if (region.size() > 1) {
                        fprintf(stderr, "\rProcessing %03.2f%% of sets",
                                (double) i_region / (double) region.size()
                                    * 100.0);
                    }
                    if (!commander.no_regress())
                        prsice.output(commander, region, i_pheno, i_region,
                                      *target_file);
                }
                if (region.size() > 1) {
                    fprintf(stderr, "\rProcessing %03.2f%% of sets\n", 100.0);
                }
            }
            prsice.summarize(commander, reporter);
        }
    }
    catch (const std::out_of_range& error)
    {
        reporter.report(error.what());
        return -1;
    }
    catch (const std::runtime_error& error)
    {
        reporter.report(error.what());
        return -1;
    }
    delete target_file;
    if (used_ld) delete ld_file;
    return 0;
}
