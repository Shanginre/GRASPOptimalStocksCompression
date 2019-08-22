#include <iostream>

#ifndef PCH_H
#define PCH_H

// TODO: add headers that you want to pre-compile here
void setCellsData(int **aCellsData, int cellsNumb);
void setMatDist(int **aMatDist, int cellsNumb);
void setValueIn2Array(int ** aArr, int row, int col, int val);
void setValueIn1Array(int * aArr, int row, int val);
void setClearSolutionArray(int ** aArr, int row, int col);
int computeProblemSolution(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolution, int **aMatAssignmentSolution);
void findGreedyRandomSolution(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int *aFreeContainersFitnessFunction, bool isOldSolution);
void setFreeContainersFitnessFunction(int **aCellsData, int **aMatDist, int cellsNumb, int *aFreeContainersFitnessFunction, int **aFreeContainersAssigntCells);
void setCurrentDist(int **aMatDist, int cellsNumb, int **aCurrentDist, int container);
void insertionSort(int **arr, int n, int dim);
int selectGoodRandomContainer(int *aFreeContainersFitnessFunction, int cellsNumb);
void improveSolutionLocalSearch(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int totalDemand);
void findBestSolutionInNeighborhoodN1(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int totalDemand, bool stayFeasible);
//void findBestSolutionInNeighborhoodN2(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int totalDemand, bool stayFeasible);
//void findBestSolutionInNeighborhoodN3(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int totalDemand, bool stayFeasible);
//void findBestSolutionInNeighborhoodN4(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int totalDemand, bool stayFeasible);
//void findBestSolutionInNeighborhoodN5(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int totalDemand, bool stayFeasible);
int getTotalPenaltyValueForContainer(int **aCellsData, int **aMatSolutionIteration, int container, int containerVolume);
int getPenaltyValueForSingleCell(int **aCellsData, int **aMatSolutionIteration, int container, int cell, int direction);
void checkCorrectnessSolution(int** aCellsData, int **aMatDist, int **aMatAssignmentSolution, int ** aMatSolution, int cellsNumb);
void delete2Array(int **m_matrix, int m_rows);
void print2Arr(int ** arr, int row, int col);
void print1Arr(int * arr, int row);
void copy2Array(int **arr1, int **arr2, int row, int col);
void computeInstanseTask(int cellsNumb);
bool isSolutionFeasible(int **aMatSolutionIteration, int **aCellsData, int cellsNumb);
void getFeasibleSolution(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration);
void removeInvalidCellsAssignment(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration);
int getSolutionTotalCosts(int ** aMatSolution, int cellsNumb);

#endif //PCH_H
