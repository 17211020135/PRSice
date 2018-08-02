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

#include "prsice.hpp"

void PRSice::get_prslice_best_threshold(const Commander& commander, Genotype& target, const size_t num_prslice_regions, const size_t pheno_index){
	// we can't do no regress here
	const int num_thread = commander.thread();
	const bool multi = pheno_info.name.size() > 1;
	const size_t num_samples_included = target.num_sample();
	const bool cumulate = commander.cumulate();


}
