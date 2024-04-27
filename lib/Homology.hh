#ifndef HOMOLOGY_INCLUDE_GUARD
#define HOMOLOGY_INCLUDE_GUARD 1

#include <utility>
#include <vector>
#include <cstddef>
#include <bitset>
#include <chrono>

#include "../threads/ThreadPool.hh"
#include "genx/Genome.hh"
#include "VariablesTypes.hh"
#include "genx/Gene.hh"
#include "genx/GenomesContainer.hh"
#include "bbh/BBHCandidatesContainer.hh"
#include <math.h>
#include <boost/unordered_set.hpp>
#include <unordered_set>
#include <iomanip>

#include "kmers/KmerMapper.hh"
#include "ScoresContainer.hh"

#include "./../utils/FileWriter.hh"
#include "./../utils/StopWatch.hh"


// #define matrixPrint
// #define candidatePrint
// #define bbhPrint



/**
 * @file Homology.hh
 * @brief Definitions for the Homology class.
 */

/**
 * @namespace homology
 * @brief Namespace containing definitions for homology computation related classes.
 */

namespace homology {

    using namespace score;
    
    /**
     * @class Homology
     * @brief Class for computing homology between genomes.
     */
    class Homology {
        private:
            
            using k_t = shared::kType;
            using index_t = shared::indexType;
            using score_t = shared::scoreType;
            using multiplicity_t = shared::multiplicityType;
            
            using kmersContainer_t = kmers::KmersContainer;
            using kmersContainer_tr = const kmersContainer_t&;
            using kmersSet_tr = const kmersContainer_t::kmerSet_tr;

            using genome_t = genome::Genome;
            using genome_tr = genome_t&;
            using genome_tp = genome_t*;
            using gene_t = gene::Gene;
            using gene_tr = const gene_t&;
            using gene_tp = gene_t*;

            
            using BBHCandidate_t = bbh::BBHCandidate;
            using BBHCandidate_tp = BBHCandidate_t*;
            using BBHCandidate_tr = BBHCandidate_t&;

            using BBHcandidatesContainer_t = bbh::BBHCandidatesContainer;
            using BBHcandidatesContainer_tp = BBHcandidatesContainer_t*;
            using BBHcandidatesContainer_tr = BBHcandidatesContainer_t&;

            using thread_pt = threads::ThreadPool;
            using thread_ptp = thread_pt*;
            using thread_ptr = thread_pt&;
            
            
            k_t k_;
            utilities::FileWriter* fw;
            std::fstream outStream_;
            thread_ptp pool_;
            std::string inFile_;


            
            /**
             * @brief Calculates the similarity values for a row using the Generalized Jaccard index.
             *        Parallel computation is performed by sending each row as a task to the ThreadPool,
             *        ensuring no concurrency issues.
             * @param rowGenes The genes in the row.
             * @param colGenes The genes in the column.
             * @param bestRows The bestRows object containing candidates columns for Bidirectional Best Hit (BBH).
             * @param scores The container for storing similarity scores.
             */
            inline void calculateRow(
                genome_t::gene_ctr rowGenes, genome_t::gene_ctr colGenes,
                BBHcandidatesContainer_tr bestRows, ScoresContainer& scores
            ) const;

            /**
             * @brief Calculates the similarity values for a row using the Generalized Jaccard index.
             *        Parallel computation is performed by sending each row as a task to the ThreadPool,
             *        ensuring no concurrency issues. This function is specialized for cases where the genome
             *        is the same, resulting in fewer comparisons to be made.
             * @param colGene The genes in the column.
             * @param bestRows The bestRows object containing candidates columns for Bidirectional Best Hit (BBH).
             * @param scores The container for storing similarity scores.
             */
            inline void calculateRowSame(genome_t::gene_ctr colGene, 
            BBHcandidatesContainer_tr bestRows, ScoresContainer& scores) const;
            

            /**
             * @brief Extracts Bidirectional Best Hits (BBH) using the similarity values calculated by calculateRow.
             * @param colGenes The genes in the column.
             * @param rowGenes The genes in the row.
             * @param candidates The container of BBH candidates (bestRows param.of calculateRow).
             * @param scores The container for storing similarity scores.
             */
            inline void
            checkForBBH(
                const genome_t::gene_ctr colGenes,
                const genome_t::gene_ctr rowGenes,
                BBHcandidatesContainer_tr candidates,
                ScoresContainer& scores
            );
            
            /**
             * @brief Extracts Bidirectional Best Hits (BBH) using the similarity values calculated by calculateRowSame.
             * @param genes The genes for which to extract BBH.
             * @param candidates The container of BBH candidates (bestRows param.of calculateRow).
             * @param scores The container for storing similarity scores.
             */
            inline void
            checkForBBHSame(
                const genome_t::gene_ctr genes,
                BBHcandidatesContainer_tr candidates,
                ScoresContainer& scores
            );

            
            /**
             * @brief Calculates the similarity between two genes using the Generalized Jaccard index.
             *        This function is used for initial filtering based on the total multiplicity of genes.
             * @param gene1 The first gene.
             * @param gene2 The second gene.
             * @return The similarity score between the two genes.
             */
            inline score_t
            calculateSimilarity(const gene_tr gene1, const gene_tr gene2) const;
            
            /**
             * @brief Calculates the similarity between two kmers containers using the Jaccard index.
             * @param gene1Container The kmers container of the first gene.
             * @param gene2Container The kmers container of the second gene.
             * @return The similarity score between the two kmers containers.
             */
            inline score_t
            calculateSimilarity(kmersContainer_tr gene1Container, kmersContainer_tr gene2Container) const;
    
            /**
             * @brief Calculates Bidirectional Best Hits (BBH) between genes of different genomes.
             * @param genome1 The first genome.
             * @param genome2 The second genome.
             */
            inline void calculateBidirectionalBestHitDifferentGenomes(genome_tr genome1, genome_tr genome2);
            
            /**
             * @brief Calculates Bidirectional Best Hits (BBH) between genes of the same genome.
             * @param genome The genome.
             */
            inline void calculateBidirectionalBestHitSameGenome(genome_tr genome);

        public:
            Homology() = delete;
            
            /**
             * @brief Constructs a Homology object with the specified parameters.
             * @param k The length of kmers.
             * @param fileName The name of the output file.
             * @param threadNumber The number of threads to use.
             */
            inline explicit Homology(k_t k, std::string fileName, ushort threadNumber);
            
            
            /**
             * @brief Constructs a Homology object with the specified parameters.
             * @param k The length of kmers.
             * @param fileName The name of the output file.
             */
            inline explicit Homology(k_t k, std::string fileName);
            
            Homology(const Homology&) = delete;
            Homology operator=(const Homology&) = delete;
            Homology(Homology&&) = delete;
            Homology& operator=(Homology&&) = delete;
            
            /**
             * @brief Destructor for Homology objects.
             */
            ~Homology() {
                fw->close(outStream_);
                delete fw;
                pool_->stop();
                delete(pool_);
            }

            /**
             * @brief Calculates Bidirectional Best Hit (BBH) for a set of genomes.
             * @param g The container of genomes.
             * @param mode The mode for recalculating kmers.
             */
            inline void calculateBidirectionalBestHit(genome::GenomesContainer& g, bool mode);
    };

    inline
    Homology::Homology(k_t k, std::string fileName, ushort threadNumber) 
    : k_(k) {
        if(k <= 0)
            throw std::runtime_error("k <= 0");
        pool_ = new thread_pt(threadNumber);
        pool_->start();
        fw = new utilities::FileWriter("", fileName, ".net", false);
        outStream_ = fw->openAppend();
    }

    inline
    Homology::Homology(k_t k, std::string fileName)
    : k_(k) {
        if(k <= 0)
            throw std::runtime_error("k <= 0");
        pool_ = new thread_pt();
        pool_->start();
        fw = new utilities::FileWriter("", fileName, ".net", false);
        outStream_ = fw->openAppend();
    }

    // 2 generalized Jaccard similarity
    // all kmers must be calculated before


    // gene1 = row gene
    inline Homology::score_t
    Homology::calculateSimilarity(const gene_tr gene1, const gene_tr gene2) const {
        

        if(
            gene1.getAlphabetLength() < gene2.getAlphabetLength()/2
            || gene2.getAlphabetLength() < gene1.getAlphabetLength()/2
        ) {
            return 0;
        }

        kmersContainer_tr shortestContainer = 
            gene1.getKmersNum() < gene2.getKmersNum() ? *gene1.getKmerContainer() : *gene2.getKmerContainer();
        kmersContainer_tr longestContainer =
            gene1.getKmersNum() < gene2.getKmersNum() ? *gene2.getKmerContainer() : *gene1.getKmerContainer();
        

        return
            // shortestContainer.getMultiplicityNumber() < longestContainer.getMultiplicityNumber()/10 ||
            // longestContainer.getMultiplicityNumber() < shortestContainer.getMultiplicityNumber()/10 ||
            // longestContainer.getSmallerKey() > shortestContainer.getBiggerKey()
            calculateSimilarity(shortestContainer, longestContainer);
    }


    inline Homology::score_t
    Homology::calculateSimilarity(kmersContainer_tr shortestContainer, kmersContainer_tr longestContainer) const {
        
        index_t longestBiggerKey = longestContainer.getBiggerKey();

        kmersSet_tr shortestSet = shortestContainer.getKmerSet();
        kmersSet_tr longestSet = longestContainer.getKmerSet();
        
        std::size_t num = 0;
        std::size_t den = 0;

        multiplicity_t currentShortestMultiplicity = 0;
        multiplicity_t currentLongestMultiplicity = 0;

        auto shortestSetBegin = shortestSet.begin();
        auto longestSetBegin = longestSet.begin();
        auto shortestSetEnd = shortestSet.end();
        auto longestSetEnd = longestSet.end();

        
        while (shortestSetBegin != shortestSetEnd && longestSetBegin != longestSetEnd) {

            index_t shortestKey = shortestSetBegin->first;
            index_t longestKey = longestSetBegin->first;
            
            if(shortestKey > longestBiggerKey) {
                break;
            }
            
            if (shortestKey < longestKey)
                ++shortestSetBegin;
            else if(shortestKey > longestKey)
                ++longestSetBegin;
            else {
                

                multiplicity_t currentKmerValue = shortestSetBegin->second;
                multiplicity_t longestVal = longestSetBegin->second;

                num += (currentKmerValue < longestVal ? currentKmerValue : longestVal);
                den += (currentKmerValue < longestVal ? longestVal : currentKmerValue);

                currentShortestMultiplicity += currentKmerValue;
                currentLongestMultiplicity += longestVal;
                
                ++shortestSetBegin;
                ++longestSetBegin;
            }
        }
        
        return 1.0*num/(den + ((shortestContainer.getMultiplicityNumber() - currentShortestMultiplicity) + (longestContainer.getMultiplicityNumber() - currentLongestMultiplicity)));
    }



    
    void Homology::calculateBidirectionalBestHit(genome::GenomesContainer& gc, bool mode) {
        if(mode) {
            genome::GenomesContainer::genome_ctr genomes = gc.getGenomes();
            auto& pool = *pool_;
            for(auto rowGenome = genomes.begin(); rowGenome != genomes.end(); ++rowGenome) {
                kmers::KmerMapper mapper;
                
                auto& rowRef = *rowGenome;
                rowRef.createAndCalculateAllKmers(k_, mapper);
                calculateBidirectionalBestHitSameGenome(rowRef);
                
                auto colGenome = rowGenome;
                ++colGenome;
                
                for(; colGenome != genomes.end(); ++colGenome){
                    colGenome->createAndCalculateAllKmers(k_, mapper);
                    calculateBidirectionalBestHitDifferentGenomes(*colGenome, rowRef);
                    colGenome->deleteAllKmers(pool);
                }

                rowRef.deleteAllKmers(pool);
            }
        } else {
            genome::GenomesContainer::genome_ctr genomes = gc.getGenomes();
            
            // Create and calculate kmers for each genome
            {
                kmers::KmerMapper mapper;
                for(auto genome = genomes.begin(); genome != genomes.end(); ++genome)
                    genome->createAndCalculateAllKmers(k_, mapper);
            }

            auto& pool = *pool_;
            
            // Compare each genome with every other genome to find BBH
            for(auto rowGenome = genomes.begin(); rowGenome != genomes.end(); ++rowGenome) {
                auto& rowRef = *rowGenome;
                calculateBidirectionalBestHitSameGenome(rowRef);
                
                auto colGenome = rowGenome;
                ++colGenome;
                
                for(; colGenome != genomes.end(); ++colGenome)
                    calculateBidirectionalBestHitDifferentGenomes(*colGenome, rowRef);
                
                rowRef.deleteAllKmers(pool);
            }
        }

    }

    
    // colGenome, rowGenome
    inline void
    Homology::calculateBidirectionalBestHitDifferentGenomes(
        genome_tr colGenome, genome_tr rowGenome
    ) {
        // std::cerr<<"\ncomparing different";

        // genes in genome1 rapresents the width of the matrix (cols), genes in genome2 rapresents the height(rows)
        genome_t::gene_ctr colGenes = colGenome.getGenes();
        genome_t::gene_ctr rowGenes = rowGenome.getGenes();


        BBHcandidatesContainer_t bestRows(rowGenes.size(), colGenes.size());

        ScoresContainer scores(rowGenes.size(), colGenes.size());

        // per la crezione della comparazione modificare qui il valore passatto usando "startCol"
        calculateRow(
            rowGenes, colGenes,
            bestRows, scores
        );
        // std::cerr<<"\npost row";
        #ifdef candidatePrint
            FileWriter fwRows("", std::to_string(rowGenome.getId())+"_"+std::to_string(colGenome.getId())+"_candidates", ".csv", false);
            auto fileRow = fwRows.openAppend();
            fwRows.write("row,col,score", fileRow);
            for(index_t i = 0; i < rowGenes.size(); ++i) {
                auto& bestCols = bestRows.getCandidateAt(i);
                auto& list = bestCols.getCandidateList();
                
                for(auto l = list.begin(); l != list.end(); ++l) {
                    fwRows.write(
                        std::to_string(rowGenes[i].getGeneFilePosition()) + "," + std::to_string(colGenes[*l].getGeneFilePosition()) + "," + std::to_string(bestCols.getBestScore()), fileRow
                    );
                }
            }
            fwRows.close(fileRow);
        
        #endif
        

        #ifdef matrixPrint
        utilities::FileWriter fw("", std::to_string(rowGenome.getId())+"_"+std::to_string(colGenome.getId())+"_matrix", ".csv", false);
        auto file = fw.openAppend();

        for(index_t col = 0; col < colGenes.size(); ++col) {
            gene_tr gene = colGenes[col];
            file<<gene.getGeneFilePosition();
            if(col != colGenes.size()-1)
                file<<",";
        }
        
        file<<"\n";
        for(index_t row = 0; row < rowGenes.size(); ++row){
            gene_tr rowGene = rowGenes[row];
            file<<rowGene.getGeneFilePosition()<<",";
            for(index_t col = 0; col < colGenes.size(); ++col) {
                // gene_tr colGene = colGenes.at(col);
                file<<scores.getScoreAt(row, col);
                if(col != colGenes.size()-1)
                    file<<",";
            }
            file<<"\n";
        }

        fw.close(file);
        #endif

        checkForBBH(
            colGenes, rowGenes,
            bestRows,
            scores
        );
    }

    // inline Homology::containerTypePointer
    inline void
    Homology::calculateBidirectionalBestHitSameGenome(
        genome_tr genome
    ) {
        // std::cerr<<"\ncomparing same";
        genome_t::gene_ctr genes = genome.getGenes();
        // genome_t::gene_ctr rowGenes = genome.getGenes();

        BBHcandidatesContainer_t bestRows(genome.size(), genome.size());
        ScoresContainer scores(genome.size(), genome.size());

        calculateRowSame(
            genes,
            bestRows, scores
        );

        #ifdef candidatePrint
            FileWriter fwRows("", std::to_string(rowGenome.getId())+"_"+std::to_string(colGenome.getId())+"_candidates", ".csv", false);
            auto fileRow = fwRows.openAppend();
            fwRows.write("row,col,score", fileRow);
            for(index_t i = 0; i < rowGenes.size(); ++i) {
                auto& bestCols = bestRows.getCandidateAt(i);
                auto& list = bestCols.getCandidateList();
                
                for(auto l = list.begin(); l != list.end(); ++l) {
                    fwRows.write(
                        std::to_string(rowGenes[i].getGeneFilePosition()) + "," +
                        std::to_string(colGenes[*l].getGeneFilePosition()) + "," +
                        std::to_string(bestCols.getBestScore()), fileRow
                    );
                }
            }
            fwRows.close(fileRow);
        
        #endif

        #ifdef matrixPrint
        utilities::FileWriter fw("", std::to_string(genome.getId())+"_"+std::to_string(genome.getId())+"_matrix", ".csv", false);
        auto file = fw.openAppend();
        
        for(index_t col = 0; col < genes.size(); ++col) {
            gene_tr gene = genes[col];
            file<<gene.getGeneFilePosition();
            if(col != genes.size()-1)
                file<<",";
        }

        file<<"\n";
        for(index_t row = 0; row < genes.size(); ++row){
            gene_tr rowGene = genes[row];
            file<<rowGene.getGeneFilePosition()<<",";
            for(index_t col = 0; col < genes.size(); ++col) {
                // gene_tr colGene = genes.at(col);
                file<<scores.getScoreAt(row, col);
                if(col != genes.size()-1)
                    file<<",";
            }
            file<<"\n";
        }

        fw.close(file);
        #endif
        
        checkForBBHSame(
            genes,
            bestRows,
            scores
        );
    }

    inline void
    Homology::calculateRowSame(
        genome_t::gene_ctr genes,
        BBHcandidatesContainer_tr bestRows, ScoresContainer& scores
    ) const {
        thread_ptr poolRef = *pool_; 
        for(index_t row = 0; row < genes.size(); ++row){
            poolRef.execute(
                [row, &scores, this, &genes, &bestRows] {
                    for(index_t col = row+1; col < genes.size(); ++col) {
                        score_t currentScore = calculateSimilarity(genes[row], genes[col]);
                        scores.setScoreAt(row, col, currentScore);
                        bestRows.addCandidate(row, currentScore, col);
                    }
                }
            );
        }
        // poolRef.waitTasks();

        while(!poolRef.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    inline void
    Homology::calculateRow(
        genome_t::gene_ctr rowGenes, genome_t::gene_ctr colGenes,
        BBHcandidatesContainer_tr bestRows, ScoresContainer& scores) const {
        
        thread_ptr poolRef = *pool_;

        for(index_t row = 0; row < rowGenes.size(); ++row){
            // gene_tr rowGene = rowGenes.at(row);
            poolRef.execute(
                [row, &scores, this, &colGenes, &bestRows, &rowGenes] {
                    gene_tr rowGene = rowGenes[row];
                    for(index_t col = 0; col < colGenes.size(); ++col) {
                        gene_tr colGene = colGenes[col];

                        score_t currentScore = calculateSimilarity(rowGene, colGene);
                        scores.setScoreAt(row, col, currentScore);
                        bestRows.addCandidate(row, currentScore, col);
                    }
                }
            );
        }
        // poolRef.waitTasks();
        while(!poolRef.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    inline void
    Homology::checkForBBH (
        const genome_t::gene_ctr colGenes, const genome_t::gene_ctr rowGenes,
        BBHcandidatesContainer_tr candidates,
        ScoresContainer &scores
    ) {
        #ifdef bbhPrint
        FileWriter fw("", std::to_string(rowGenes[0].getGenomeId())+"_"+std::to_string(colGenes[0].getGenomeId())+"_bbh", ".csv", false);
        auto file = fw.openAppend();
        fw.write("row,col,score",file);
        #endif

        auto matchp = candidates.getPossibleMatch(); 
        auto& match = *matchp; 
        
        // BBHcandidatesContainer_t::set_tr matchRef = *match;
        // bbh::BidirectionalBestHitContainer& bbhContainerRef = *bbh;
        // passo per tutte le colonne "candidate"
        auto& poolRef = *pool_;
        for(auto col = match.begin(); col != match.end(); ++col) {
            const auto& currentColRef = *col;
            
            #ifdef bbhPrint

                poolRef.execute(
                    [&currentColRef, &colGenes, &rowGenes, &scores, &candidates, &bbhContainerRef, &fw, &file] {
                        
                        score_t bestScore = -1;
                        
                        std::unordered_set<index_t> currentBestIndexs;
                        boost::unordered_set<index_t> currentBestIndexs;
                        index_t colGeneId = currentColRef.first;
                        gene_tr currentColGene = colGenes[colGeneId];
                        // estrae le migliori righe per la colonna corrente
                        // e li memorizza in current best indexs

                        for(index_t row = 0; row < rowGenes.size(); ++row) {
                            score_t currentScore = scores.getScoreAt(row, colGeneId);

                            if(currentScore > bestScore) {
                                bestScore = currentScore;
                                currentBestIndexs.clear();
                                currentBestIndexs.insert(row);
                            } else if(currentScore == bestScore) {
                                currentBestIndexs.insert(row);
                            }
                        }

                        // passa per tutte le righe con il punteggio migliore
                        // e se quel punteggio è il migliore anche per la riga
                        // crea il bbh
                        if(bestScore > 0) {
                            for(auto index = currentBestIndexs.begin(); index != currentBestIndexs.end(); ++index) {
                                index_t currentIndex = *index;
                                
                                if(bestScore == candidates.getBestScoreForCandidate(currentIndex)) {
                                    
                                    bbhContainerRef.add (
                                        currentColGene.getGenomeId(),
                                        rowGenes[currentIndex].getGenomeId(),
                                        colGeneId, currentIndex, bestScore,
                                        colGenes[colGeneId].getGeneFilePosition(),
                                        rowGenes[currentIndex].getGeneFilePosition()
                                    );
                                    
                                    if(colGenes[colGeneId].getGeneFilePosition() != rowGenes[currentIndex].getGeneFilePosition())
                                        fw.write(
                                            std::to_string(rowGenes[currentIndex].getGeneFilePosition()) + "," +
                                            std::to_string(colGenes[colGeneId].getGeneFilePosition()) + "," +
                                            std::to_string(bestScore),
                                            file
                                        );
                                }
                            }
                        }
                    }
                );
            #endif


            #ifndef bbhPrint
                poolRef.execute(
                    [&currentColRef, &colGenes, &rowGenes, &scores, &candidates, this] {
                        
                        auto& fwRef = *fw;
                        score_t bestScore = -1;
                        
                        // std::unordered_set<index_t> currentBestIndexs;
                        boost::unordered_set<index_t> currentBestIndexs;
                        index_t colGeneId = currentColRef.first;
                        gene_tr currentColGene = colGenes[colGeneId];
                        // estrae le migliori righe per la colonna corrente
                        // e li memorizza in current best indexs

                        for(index_t row = 0; row < rowGenes.size(); ++row) {
                            score_t currentScore = scores.getScoreAt(row, colGeneId);

                            if(currentScore > bestScore) {
                                bestScore = currentScore;
                                currentBestIndexs.clear();
                                currentBestIndexs.insert(row);
                            } else if(currentScore == bestScore) {
                                currentBestIndexs.insert(row);
                            }
                        }

                        // passa per tutte le righe con il punteggio migliore
                        // e se quel punteggio è il migliore anche per la riga
                        // crea il bbh
                        index_t currentColGeneFileLine = currentColGene.getGeneFilePosition();
                        for(auto index = currentBestIndexs.begin(); index != currentBestIndexs.end(); ++index) {
                            index_t currentIndex = *index;
                            
                            if(bestScore == candidates.getBestScoreForCandidate(currentIndex)) {
                                fwRef.write(
                                    std::to_string(
                                        rowGenes[currentIndex].getGeneFilePosition()
                                    ) + "," +
                                    std::to_string(
                                        currentColGeneFileLine
                                    ) + "," +
                                    std::to_string(bestScore)
                                    , outStream_);
                            }
                        }
                        // if(bestScore > 0) {
                        // }
                    }
                );
            #endif
            
        }
        // poolRef.waitTasks();

        while(!poolRef.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        #ifdef bbhPrint
        fw.close(file);
        #endif
        delete matchp;
    }

    inline void
    Homology::checkForBBHSame (
        const genome_t::gene_ctr genes, 
        BBHcandidatesContainer_tr candidates,
        ScoresContainer& scores
    ) {
        #ifdef bbhPrint
        FileWriter fw("", std::to_string(genes[0].getGenomeId())+"_"+std::to_string(genes[0].getGenomeId())+"_bbh", ".csv", false);
        auto file = fw.openAppend();
        fw.write("row,col,score",file);
        #endif
        
        auto matchp = candidates.getPossibleMatch(); 
        auto& match = *matchp; 
        // BBHcandidatesContainer_t::set_tr matchRef = *match;
        
        // bbh::BidirectionalBestHitContainer& bbhContainerRef = *bbh;
        auto& poolRef = *pool_;
        
        // passo per tutte le colonne "candidate"
        for(auto col = match.begin(); col != match.end(); ++col) {
            const auto& currentColRef = *col;
            #ifdef bbhPrint

                poolRef.execute(
                    [&currentColRef, &genes, &scores, &candidates, &bbhContainerRef, &file, &fw] {
                        score_t bestScore = -1;
                
                        // std::unordered_set<index_t> currentBestIndexs;
                        boost::unordered_set<index_t> currentBestIndexs;
                        index_t colGeneId = currentColRef.first;
                        gene_tr currentColGene = genes[colGeneId];
                        // estrae le migliori righe per la colonna corrente
                        // e li memorizza in current best indexs

                        // le prime righe fino alla diagonale
                        for(index_t row = 0; row < colGeneId; ++row) {
                            score_t currentScore = scores.getScoreAt(row, colGeneId);

                            if(currentScore > bestScore) {
                                bestScore = currentScore;
                                currentBestIndexs.clear();
                                currentBestIndexs.insert(row);
                            } else if(currentScore == bestScore) {
                                currentBestIndexs.insert(row);
                            }
                        }

                        // passa per tutte le righe con il punteggio migliore
                        // e se quel punteggio è il migliore anche per la riga
                        // crea il bbh

                        if(bestScore > 0)
                            for(auto index = currentBestIndexs.begin(); index != currentBestIndexs.end(); ++index) {
                                index_t currentIndex = *index;

                                if(bestScore == candidates.getBestScoreForCandidate(currentIndex)) {
                                    bbhContainerRef.add (
                                        currentColGene.getGenomeId(),
                                        genes[currentIndex].getGenomeId(),
                                        colGeneId, currentIndex, bestScore,
                                        genes[colGeneId].getGeneFilePosition(),
                                        genes[currentIndex].getGeneFilePosition()
                                    );
                                    if(genes[colGeneId].getGeneFilePosition() != genes[currentIndex].getGeneFilePosition())
                                        fw.write(
                                            std::to_string(genes[currentIndex].getGeneFilePosition()) + "," +
                                            std::to_string(genes[colGeneId].getGeneFilePosition()) + "," +
                                            std::to_string(bestScore),
                                            file
                                        );
                                }
                            }
                    }
                );
            #endif
            
            #ifndef bbhPrint
                poolRef.execute(
                    [&currentColRef, &genes, &scores, &candidates, this] {
                        auto& fwRef = *fw;
                        score_t bestScore = -1;

                        boost::unordered_set<index_t> currentBestIndexs;
                        // std::unordered_set<index_t> currentBestIndexs;
                        index_t colGeneId = currentColRef.first;
                        gene_tr currentColGene = genes[colGeneId];
                        // estrae le migliori righe per la colonna corrente
                        // e li memorizza in current best indexs

                        // le prime righe fino alla diagonale
                        for(index_t row = 0; row < colGeneId; ++row) {
                            score_t currentScore = scores.getScoreAt(row, colGeneId);

                            if(currentScore > bestScore) {
                                bestScore = currentScore;
                                currentBestIndexs.clear();
                                currentBestIndexs.insert(row);
                            } else if(currentScore == bestScore) {
                                currentBestIndexs.insert(row);
                            }
                        }

                        // passa per tutte le righe con il punteggio migliore
                        // e se quel punteggio è il migliore anche per la riga
                        // crea il bbh

                        // if(bestScore > 0)
                        index_t currentColGeneFileLine = currentColGene.getGeneFilePosition();
                        for(auto index = currentBestIndexs.begin(); index != currentBestIndexs.end(); ++index) {
                            index_t currentIndex = *index;

                            if(bestScore == candidates.getBestScoreForCandidate(currentIndex)) {
                                fwRef.write(
                                    std::to_string(
                                        genes[currentIndex].getGeneFilePosition()
                                    ) + "," +
                                    std::to_string(
                                        currentColGeneFileLine
                                    ) + "," +
                                    std::to_string(bestScore)
                                    , outStream_);
                                
                            }
                        }
                    }
                );
            #endif
        }
        // poolRef.waitTasks();

        while(!poolRef.tasksCompleted()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        #ifdef bbhPrint
        fw.close(file);
        #endif
        delete matchp;
    }
    
}

#endif
