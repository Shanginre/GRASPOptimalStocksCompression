#include "pch.h"
#include <iostream>
#include <algorithm>

using namespace std;

int main()
{
	int cellsNumb = 10;
	
	computeInstanseTask(cellsNumb);
}

void computeInstanseTask(int cellsNumb) {

	// Массив данных по ячейкам: [i][0] вместимость, [i][1] имеющийся изначальный объем товара в ячейке, [i][2] затраты на выбор контейнера
	// [0], [1] в кубических дециметрах (т.е. кубик 10 см). Данные объемов хранятся в базе WMS с точностью 3 знака, так что мы ничего не потеряем при приобразовании
	int **aCellsData = new int*[cellsNumb];
	for (int count = 0; count < cellsNumb; count++)
		aCellsData[count] = new int[3];

	setCellsData(aCellsData, cellsNumb);

	// Матрица затрат на перемещение между ячейками. Номер строки/столбца соответствут номеру ячейки
	// [i][j] -- затраты на перемещение из ячейки j в контейнер i
	int **aMatDist = new int*[cellsNumb];
	for (int count = 0; count < cellsNumb; count++)
		aMatDist[count] = new int[cellsNumb];

	setMatDist(aMatDist, cellsNumb);

	// Матрицы решения
	// aMatSolution - массив контейнеров в решениии: 
	// [i][0] номер контейнера, [i][1] текущий объем товара в контейнере, [i][2] текущие затраты на выбор контейнера и прикрепление к нему ячеек
	int **aMatSolution = new int*[cellsNumb];
	for (int count = 0; count < cellsNumb; count++)
		aMatSolution[count] = new int[3];
	setClearSolutionArray(aMatSolution, cellsNumb, 3);

	// aMatAssignmentSolution - матрица привязки доноров к контейнерам. [i][j] = 1 - если j привязан к i, 0 в противном случае
	int **aMatAssignmentSolution = new int*[cellsNumb];
	for (int count = 0; count < cellsNumb; count++)
		aMatAssignmentSolution[count] = new int[cellsNumb];
	setValueIn2Array(aMatAssignmentSolution, cellsNumb, cellsNumb, 0); // заполняем нулями матрицу

	int totalCostsSolution = computeProblemSolution(aCellsData, aMatDist, cellsNumb, aMatSolution, aMatAssignmentSolution);

	delete2Array(aCellsData, cellsNumb);
	delete2Array(aMatDist, cellsNumb);
	delete2Array(aMatSolution, cellsNumb);
	delete2Array(aMatAssignmentSolution, cellsNumb);

}

void setCellsData(int **aCellsData, int cellsNumb) {
	
	for (int i = 0; i < cellsNumb; i++) {
		aCellsData[i][0] = 30 + rand()%20;
		aCellsData[i][1] = 2 + rand() % 20;
		aCellsData[i][2] = aCellsData[i][0] * 10 + 1500;
	}
}

void setMatDist(int **aMatDist, int cellsNumb) {
	setValueIn2Array(aMatDist, cellsNumb, cellsNumb, 0); // заполняем нулями матрицу
	
	for (int i = 0; i < cellsNumb; i++) {
		aMatDist[i][i] = 0;
		for (int j = i + 1; j < cellsNumb; j++) {
			aMatDist[i][j] = 1 + rand() % 40;
			aMatDist[j][i] = aMatDist[i][j];
		}
	}
}

void setValueIn2Array(int ** aArr, int row, int col, int val) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			aArr[i][j] = val;
		}
	}
}

void setClearSolutionArray(int ** aArr, int row, int col) {
	for (int i = 0; i < row; i++) {
		aArr[i][0] = -1;
		for (int j = 1; j < col; j++) {
			aArr[i][j] = 0;
		}
	}
}

void setValueIn1Array(int * aArr, int row, int val) {
	for (int i = 0; i < row; i++) {
		aArr[i] = val;
	}
}

int computeProblemSolution(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolution, int **aMatAssignmentSolution) {
	
	// Матрицы текущего решения на некоторой итерации

	// aMatSolutionIteration - массив контейнеров в решениии: 
	// [i][0] номер контейнера, [i][1] текущий объем товара в контейнере, [i][2] текущие затраты и штраф на выбор контейнера и прикрепление к нему ячеек
	// [i][3] отдельно величина штрафа за превышение объема контейнера
	int **aMatSolutionIteration = new int*[cellsNumb];
	for (int count = 0; count < cellsNumb; count++)
		aMatSolutionIteration[count] = new int[4];
	
	// aMatAssignmentSolutionIteration - матрица привязки доноров к контейнерам. [i][j] = 1 - если j привязан к i, 0 в противном случае
	int **aMatAssignmentSolutionIteration = new int*[cellsNumb];
	for (int count = 0; count < cellsNumb; count++)
		aMatAssignmentSolutionIteration[count] = new int[cellsNumb];
	

	const int maxIteration = 10;
	int iter = 0, recordCosts = 1000000;

	// определим общий объем товаров, которые необходимо разместить в контейнерах
	int totalDemand = 0;
	for (int i = 0; i < cellsNumb; i++)
		totalDemand += aCellsData[i][1];

	while (iter < maxIteration) {
		
		// Обнулим массивы временного решения
		setValueIn2Array(aMatAssignmentSolutionIteration, cellsNumb, cellsNumb, 0); // заполняем нулями матрицу
		setClearSolutionArray(aMatSolutionIteration, cellsNumb, 4);

		// Строим решение рандомизированным жадным алгоритмом
		int *aFreeContainersFitnessFunction = new int[cellsNumb]; // пока пустой массив, так как решение строим с нуля
		findGreedyRandomSolution(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, aFreeContainersFitnessFunction, false);

		// Улучшаем решение алгоритмом локального поиска по ряду окрестностей
		improveSolutionLocalSearch(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, totalDemand);
		
		// Проверяем решение на допустимость (чтобы все ограничения по вместимости контейнеров соблюдались)
		bool feasible = isSolutionFeasible(aMatSolutionIteration, aCellsData, cellsNumb);

		// Если решение не допустимо, то исправить решение
		if (feasible == false) {
			getFeasibleSolution(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration);
		}
		
		// Если затраты найденного решения на текущей итерации меньше, чем рекорд, то обновим рекорд и запишем это решение в итоговое
		int totalCostsIteration = getSolutionTotalCosts(aMatSolutionIteration, cellsNumb);
		if (totalCostsIteration < recordCosts) {
			recordCosts = totalCostsIteration;
			copy2Array(aMatSolution, aMatSolutionIteration, cellsNumb, 3);
		}
		
		iter++;
	}

	delete2Array(aMatSolutionIteration, cellsNumb);
	delete2Array(aMatAssignmentSolutionIteration, cellsNumb);

	return recordCosts;
}

void findGreedyRandomSolution(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int *aFreeContainersFitnessFunction, bool isOldSolution) {
	
	// количество ячеек в решении
	int numCellsInSolution = 0;

	if (isOldSolution) {
		// если это уже существующее решение, то перезаполним aFreeContainersFitnessFunction
		// переопределим numCellsInSolution так, чтобы она содержала количество прикрепленных ячеек
		for (int i = 0; i < cellsNumb; i++) {
			for (int j = 0; j < cellsNumb; j++) {
				if (aMatAssignmentSolutionIteration[i][j] == 1)
					numCellsInSolution++;
			}
		}
	}

	// множество ячеек, которые планируется прикрепить к контейнеру из массива aFreeContainersFitnessFunction, в случае его выбора
	// [i][j] = 1, если ячейка j прикрепляется к контейнеру i, 0 иначе
	int **aFreeContainersAssigntCells = new int*[cellsNumb];
	for (int count = 0; count < cellsNumb; count++)
		aFreeContainersAssigntCells[count] = new int[cellsNumb];
	

	while (numCellsInSolution != cellsNumb) {
		setValueIn2Array(aFreeContainersAssigntCells, cellsNumb, cellsNumb, 0); // заполняем нулями матрицу

		// вычислим для каждого не выбранного контейнера значение функции его привлекательности для включение в решение
		setFreeContainersFitnessFunction(aCellsData, aMatDist, cellsNumb, aFreeContainersFitnessFunction, aFreeContainersAssigntCells);

		// формируем множество контейнеров, удовлетворяющих пороговому критерию
		// и случайным образом выбираем контейнер из множества, сформированного по пороговому критерию и включаем его в решение
		int selectedRandomContainer = selectGoodRandomContainer(aFreeContainersFitnessFunction, cellsNumb);

		// включим контейнер и прикрепленные к нему ячейки в решение
		aMatSolutionIteration[selectedRandomContainer][0] = selectedRandomContainer;
		aMatSolutionIteration[selectedRandomContainer][1] = 0;
		aMatSolutionIteration[selectedRandomContainer][2] = aCellsData[selectedRandomContainer][2];
		for (int j = 0; j < cellsNumb; j++) {
			if (aFreeContainersAssigntCells[selectedRandomContainer][j] == 1) {					
				aMatAssignmentSolutionIteration[selectedRandomContainer][j] = 1;
				aMatSolutionIteration[selectedRandomContainer][1] += aCellsData[j][1];
				aMatSolutionIteration[selectedRandomContainer][2] += aMatDist[selectedRandomContainer][j];
				aMatSolutionIteration[selectedRandomContainer][3] = 0;

				aFreeContainersFitnessFunction[j] = 10000;
				numCellsInSolution++;
			}
		}
	}

	delete2Array(aFreeContainersAssigntCells, cellsNumb);
	delete[] aFreeContainersFitnessFunction;
}

void setFreeContainersFitnessFunction(int **aCellsData, int **aMatDist, int cellsNumb, int *aFreeContainersFitnessFunction, int **aFreeContainersAssigntCells) {

	// массив ячеек, упорядоченных по возрастанию удаленности от текущего контейнера i
	// [0][k] - расстояние от ячейки до текущего контейнера, [1][k] - номер ячейки
	int **aCurrentDist = new int*[2];
	for (int count = 0; count < 2; count++)
		aCurrentDist[count] = new int[cellsNumb];

	for (int i = 0; i < cellsNumb; i++) {
		// 10000 - если ячейка уже выбрана в решение, то она не перевыбирается на этом этапе
		if (aFreeContainersFitnessFunction[i] >= 10000) continue; 

		// текущий общий объем товаров в контейнере
		int containerCurrentVolume = aCellsData[i][1];

		// общие затраты на выбор контейнера
		int containerCosts = aCellsData[i][2];

		// число ячеек-доноров, прикрепленных к контейнеру i
		int amountAssignedCell = 0;
		
		setCurrentDist(aMatDist, cellsNumb, aCurrentDist, i);

		for (int j = 0; j < cellsNumb; j++) {
			int currentCell = aCurrentDist[1][j];
			
			if (currentCell == i) continue;
			if (aFreeContainersFitnessFunction[currentCell] >= 10000) continue;
			if (aCellsData[i][0] < containerCurrentVolume + aCellsData[currentCell][1]) continue;

			aFreeContainersAssigntCells[i][currentCell] = 1;
			containerCosts += aCurrentDist[0][j];
			containerCurrentVolume += aCellsData[currentCell][1];
			amountAssignedCell++;
		}

		//товар в контейнере в любом случае назначаем в тот же контейнер (то есть он остается где был)
		if (aCellsData[i][1] > 0) {
			aFreeContainersAssigntCells[i][i] = 1;
			amountAssignedCell++;
		}
		
		// если количество потенциально прикрепляемых ячеек больше 1 (то есть мы уменьшаем количество ячеек), то тогда увеличим привлекательность такого решения
		// по сравнению с теми решениями где количество прикрепляемых ячеек равно 1.
		// таким образом мы сначала отбираем контейнеры с наибольшим количеством ячеек. В противном случае может получиться ситуация, когда выбор маленького контейнера
		// где уже находится товар (прикрепляется сам к себе) будет более привлекательным чем выбор большого контейнера к которому прикрепляются несколько ячеек.
		// Если выбрать такой маленький контейнер в самом начале (и исключить его из дальнейшего рассмотрения), то мы исключаем возможность дальнейшее назначение его 
		// в другие контейнеры, а значит исключаем возможность уменьшения количество контейнеров и построения более качественного жадного решения.
		if (amountAssignedCell > 1)
			amountAssignedCell *= 10;

		if (amountAssignedCell > 0) 
			aFreeContainersFitnessFunction[i] = containerCosts / amountAssignedCell;
		else 
			aFreeContainersFitnessFunction[i] = 10000;
	}

	delete2Array(aCurrentDist, 2);

}

void setCurrentDist(int **aMatDist, int cellsNumb, int **aCurrentDist, int container){
	
	for (int j = 0; j < cellsNumb; j++) {
		aCurrentDist[0][j] = aMatDist[container][j];
		aCurrentDist[1][j] = j;
	}
	
	// сортируем массив по возрастанию расстояния до контейнера
	insertionSort(aCurrentDist, cellsNumb, 1);
}

void insertionSort(int **arr, int n, int dim) {

	int key, j;
	int *id = new int[dim+1];

	for (int i = 1; i < n; i++)
	{
		key = arr[0][i];
		for (int d = 1; d <= dim; d++)
			id[d] = arr[d][i];

		j = i - 1;

		while (j >= 0 && arr[0][j] > key)
		{
			arr[0][j + 1] = arr[0][j];
			for (int d = 1; d <= dim; d++)
				arr[d][j + 1] = arr[d][j];
			j = j - 1;
		}
		arr[0][j + 1] = key;
		for (int d = 1; d <= dim; d++)
			arr[d][j + 1] = id[d];
	}

	delete[] id;
}

int selectGoodRandomContainer(int *aFreeContainersFitnessFunction, int cellsNumb) {

	int minFit = 10000;
	for (int i = 0; i < cellsNumb; i++) {
		if (minFit > aFreeContainersFitnessFunction[i])
			minFit = aFreeContainersFitnessFunction[i];
	}

	int threshold = minFit * (1.2); // 20 % увеличение стоимости решение это порог
	
	// массив номеров контейнеров удовлетворяющих пороговому критерию
	int *aFreeContainersThresholdFitnessFunction = new int[cellsNumb];

	int containerNumber = 0;
	for (int i = 0; i < cellsNumb; i++) {
		if (threshold >= aFreeContainersFitnessFunction[i] && aFreeContainersFitnessFunction[i] < 10000) {
			aFreeContainersThresholdFitnessFunction[containerNumber] = i;
			containerNumber++;
		}
	}

	int randomNumber = rand() % containerNumber;
	int randomContainer = aFreeContainersThresholdFitnessFunction[randomNumber];
	delete[] aFreeContainersThresholdFitnessFunction;
	return randomContainer;
}

void improveSolutionLocalSearch(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int totalDemand) {

	// Найденное жадное решение улучшаем алгоритмом локального поиска, последовательно перебирая окрестности

	// Строим множество соседних решений по окрестности N1 от текущего решения и выбираем лучшее решение в окрестности
	findBestSolutionInNeighborhoodN1(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, totalDemand, false);

	//// Строим множество соседних решений по окрестности N2 от текущего решения и выбираем лучшее решение в окрестности
	//findBestSolutionInNeighborhoodN2(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, totalDemand, false);

	//// Строим множество соседних решений по окрестности N3 от текущего решения и выбираем лучшее решение в окрестности
	//findBestSolutionInNeighborhoodN3(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, totalDemand, false);

	//// Строим множество соседних решений по окрестности N4 от текущего решения и выбираем лучшее решение в окрестности
	//findBestSolutionInNeighborhoodN4(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, totalDemand, false);

	//// Строим множество соседних решений по окрестности N5 от текущего решения и выбираем лучшее решение в окрестности
	//findBestSolutionInNeighborhoodN5(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, totalDemand, false);

}

void findBestSolutionInNeighborhoodN1(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration, int totalDemand, bool stayFeasible) {

	// Значения для найденного рекордного решения в окрестности
	int recordDeltaCosts, recordCell, recordNewContainer, recordNewCosts, recordNewPenalty, recordOldContainer, recordOldCosts, recordOldPenalty;

	do {	
		recordDeltaCosts = 10000;

		int totalContainersCapacity = 0;
		for (int i = 0; i < cellsNumb; i++)
			if (aMatSolutionIteration[i][1] > 0)
				totalContainersCapacity += aCellsData[i][0];

		// перебираем все ячейки-доноры
		for (int j = 0; j < cellsNumb; j++) {
			// Получим текущий контейнер, к которому прикреплена ячейка j
			int currentContainer;
			for (int i = 0; i < cellsNumb; i++) {
				if (aMatAssignmentSolutionIteration[i][j] == 1) {
					currentContainer = i;
					break;
				}
			}
			
			// получим число ячеек, которые прикреплены к контейнеру j (если прикреплены вообще)
			int numbAssignedCells = 0;
			if (aMatSolutionIteration[j][0] >= 0) {	
				for (int i = 0; i < cellsNumb; i++) {
					if (aMatAssignmentSolutionIteration[j][i] == 1) {
						numbAssignedCells = i;
					}
				}
			}
			else {
				numbAssignedCells = 0;
			}
			// Если остатки ячейки j размещаются сами в себя и есть другие ячейки, которые прикреплены к контейнеру j, то такую ячейку исключаем из рассмотрения
			// это необходимо чтобы избежать "чехорды" по словам заказчика и упрощает физическое выполнение складского процесса сжатия остатков, когда есть сжатие партий
			if (j == currentContainer && numbAssignedCells > 1) {
				continue;
			}

			// текущее значение общей вместимости контейнеров с учетов перестановок
			int currentTotalContainersCapacity = totalContainersCapacity - aCellsData[currentContainer][0];

			// Получаем затраты, которые мы экономим, если не будем перемещать остатки из ячейки j в тукущий контейнер
			int currentCosts = aMatDist[currentContainer][j];
			// Если после отмены перемещения остатков из ячейки j в текущий контейнер у нас текущий контейнер стал пустой, то учтем эту экономию затрат
			if (aMatSolutionIteration[currentContainer][1] - aCellsData[j][1] == 0)
				currentCosts += aCellsData[currentContainer][2];
			// если после отмены перемещения остатков из ячейки j в текущий контейнер у нас величина штрафа уменьшилась, то учтем эту экономию затрат
			int currentPenelty = getPenaltyValueForSingleCell(aCellsData, aMatSolutionIteration, currentContainer, j, 0);
			currentCosts += currentPenelty;

			for (int i = 0; i < cellsNumb; i++) {
				if (i == currentContainer)
					continue;

				if (stayFeasible) {
					if (max(0, aMatSolutionIteration[i][1]) + aCellsData[j][1] > aCellsData[i][0])
						continue;
				}
				else {
					if (currentTotalContainersCapacity + aCellsData[i][0] < totalDemand)
						continue;
				}

				// новые затраты на перемещение
				int newCosts = aMatDist[i][j];
				// новые затраты на выбор контейнера
				if (aMatSolutionIteration[i][0] == -1)
					newCosts += aCellsData[i][2];
				// новые затраты на штраф за превышение объема
				int newPenalty = getPenaltyValueForSingleCell(aCellsData, aMatSolutionIteration, i, j, 1);
				newCosts += newPenalty;

				int deltaCosts = newCosts - currentCosts;
				if (deltaCosts < 0 && deltaCosts < recordDeltaCosts) {
					recordDeltaCosts = deltaCosts;
					recordCell = j;
					recordNewContainer = i;
					recordNewCosts = newCosts;
					recordNewPenalty = newPenalty;
					recordOldContainer = currentContainer;
					recordOldCosts = currentCosts;
					recordOldPenalty = currentPenelty;
				}
			}
		}

		// если улучшение произошло, то заменим текущее решение на рекордное
		if (recordDeltaCosts < 0) {
			// уберем из текущего решения информацию о назначении старой ячейки в старый контейнер
			aMatSolutionIteration[recordOldContainer][1] -= aCellsData[recordCell][1];
			aMatSolutionIteration[recordOldContainer][2] -= recordOldCosts;
			if (aMatSolutionIteration[recordOldContainer][1] == 0)
				aMatSolutionIteration[recordOldContainer][0] = -1;
			aMatSolutionIteration[recordOldContainer][3] -= recordOldPenalty;

			aMatAssignmentSolutionIteration[recordOldContainer][recordCell] = 0;

			// добавим в текущее решение информацию о назначении новой ячейки в новый контейнер
			aMatSolutionIteration[recordNewContainer][1] += aCellsData[recordCell][1];
			aMatSolutionIteration[recordNewContainer][2] += recordNewCosts;
			if (aMatSolutionIteration[recordNewContainer][0] == -1)
				aMatSolutionIteration[recordNewContainer][0] = recordNewContainer;
			aMatSolutionIteration[recordNewContainer][3] += recordNewPenalty;

			aMatAssignmentSolutionIteration[recordNewContainer][recordCell] = 1;

			//checkCorrectnessSolution(aCellsData, aMatDist, aMatAssignmentSolutionIteration, aMatSolutionIteration, cellsNumb);
		}
	} while (recordDeltaCosts < 0);

}

int getTotalPenaltyValueForContainer(int **aCellsData, int **aMatSolutionIteration, int container, int containerVolume) {
	int value = (aCellsData[container][2] / aCellsData[container][0]) * max(0, containerVolume - aCellsData[container][0]);
	return value;
}

int getPenaltyValueForSingleCell(int **aCellsData, int **aMatSolutionIteration, int container, int cell, int direction) {
	// переменные необходимые для расчета штрафа текущего и нового решений
	int oldPenalty, newVolume, newPenalty;

	oldPenalty = max(0, aMatSolutionIteration[container][3]);
	// direction = 0 исключаем ячейку из контейнера, иначе если = 1, то включаем ее
	if (direction == 0)
		newVolume = max(0, aMatSolutionIteration[container][1] - aCellsData[cell][1]);
	else
		newVolume = max(0, aMatSolutionIteration[container][1] + aCellsData[cell][1]);
	newPenalty = getTotalPenaltyValueForContainer(aCellsData, aMatSolutionIteration, container, newVolume);
	if (direction == 0)
		return oldPenalty - newPenalty;
	else
		return newPenalty - oldPenalty;
}

void checkCorrectnessSolution(int** aCellsData, int **aMatDist, int **aMatAssignmentSolution, int ** aMatSolution, int cellsNumb) {

	int totalDemand = 0;
	int totalAssignedDemand = 0;
	int totalCapacity = 0;

	for (int i = 0; i < cellsNumb; i++) {
		totalDemand += aCellsData[i][1];
		
		if (aMatSolution[i][0] == -1)
			continue;

		int costs = aCellsData[i][2];
		int volume = 0;
		for (int j = 0; j < cellsNumb; j++) {
			if (aMatAssignmentSolution[i][j] == 1) {
				costs += aMatDist[i][j];
				volume += aCellsData[j][1];
			}
		}
		
		// проврка на соответствие затрат
		if (costs != aMatSolution[i][2] - aMatSolution[i][3])
			cout << "!!!!!!!!!!!! costs does not match: " << i << " on " << costs - aMatSolution[i][2] << endl;

		// проверка на соответствие объемов
		if (volume != aMatSolution[i][1])
			cout << "!!!!!!!!!!!! volume does not match : " << i << " on " << costs - aMatSolution[i][2] << endl;

		// проверка на штрафы
		int penalty = getTotalPenaltyValueForContainer(aCellsData, aMatSolution, i, volume);
		if (penalty != aMatSolution[i][3])
			cout << "!!!!!!!!!!!! penalty does not match : " << i << " on " << penalty - aMatSolution[i][3] << endl;

		// проверка на соответствие объемов
		if (volume > aCellsData[i][0])
			cout << "!!!!!!!!!!!! volume excess : " << i << " on " << volume - aCellsData[i][0] << endl;

		totalCapacity += aCellsData[i][0];
		totalAssignedDemand += aMatSolution[i][1];
	}
	
	// проверка на вместимость остатков во все контейнеры
	if (totalDemand > totalCapacity)
		cout << "!!!!!!!!!!!! totalDemand excess on " << totalDemand - totalCapacity << endl;
	
	// проверка на назначение всех объемов в контейнеры
	if (totalDemand != totalAssignedDemand)
		cout << "!!!!!!!!!!!! totalAssignedDemand excess on " << totalDemand - totalAssignedDemand << endl;
	
	// проверка на прикрепление только к одному контейнеру
	
	for (int j = 0; j < cellsNumb; j++) {
		int CellCount = 0;
		for (int i = 0; i < cellsNumb; i++) {
			CellCount += aMatAssignmentSolution[i][j];
		}
		if (CellCount > 1) {
			std::cout << "cell " << j << "is assigned, times " << CellCount << endl;
		}
	}

}

void delete2Array(int **m_matrix, int m_rows) {
	for (int i = 0; i < m_rows; i++)
		delete[] m_matrix[i];

	delete[] m_matrix;
}

void print2Arr(int ** arr, int row, int col) {

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			std::cout << arr[i][j] << "  ";
		}
		std::cout << endl;
	}

	std::cout << endl;
	std::cout << endl;
}

void print1Arr(int * arr, int row) {

	for (int i = 0; i < row; i++) {
		std::cout << arr[i] << "  ";
	}

	std::cout << endl;
	std::cout << endl;
}

void copy2Array(int **arr1, int **arr2, int row, int col) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			arr1[i][j] = arr2[i][j];
		}
	}
}

bool isSolutionFeasible(int **aMatSolutionIteration, int **aCellsData, int cellsNumb) {
	bool feasible = true;
	for (int i = 0; i < cellsNumb; i++) {
		if (aMatSolutionIteration[i][1] > aCellsData[i][0])
			feasible = false;
	}
	return feasible;
}

void getFeasibleSolution(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration) {

	// удалим ячейки из решения, из-за которых появляется превышение объемов в контейнерах
	removeInvalidCellsAssignment(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration);

	// не прикрепленные ячейки прикрепим к контейнерам по жадному алгоритму

	// определим значения массива функции привлекательности для свободных контейнеров. 
	// Значение 10000 - значит контейнер или ячейка уже в решении и их нельзя перевыбирать
	int *aFreeContainersFitnessFunction = new int[cellsNumb];
	for (int i = 0; i < cellsNumb; i++) {
		for (int j = 0; j < cellsNumb; j++) {
			if (aMatAssignmentSolutionIteration[i][j] == 1)
				aFreeContainersFitnessFunction[j] = 10000;
		}
	}

	//checkCorrectnessSolution(aCellsData, aMatDist, aMatAssignmentSolutionIteration, aMatSolutionIteration, cellsNumb);

	findGreedyRandomSolution(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, aFreeContainersFitnessFunction, true);

	checkCorrectnessSolution(aCellsData, aMatDist, aMatAssignmentSolutionIteration, aMatSolutionIteration, cellsNumb);
	// улучшим решение процедурой локального поиска по ряду окрестностей, но с ограничением на вместимость контейнера

	// Строим множество соседних решений по окрестности N1 от текущего решения и выбираем лучшее решение в окрестности
	findBestSolutionInNeighborhoodN1(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, 0, true);

	//// Строим множество соседних решений по окрестности N2 от текущего решения и выбираем лучшее решение в окрестности
	//findBestSolutionInNeighborhoodN2(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, 0, true);

	//// Строим множество соседних решений по окрестности N3 от текущего решения и выбираем лучшее решение в окрестности
	//findBestSolutionInNeighborhoodN3(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, 0, true);

	//// Строим множество соседних решений по окрестности N4 от текущего решения и выбираем лучшее решение в окрестности
	//findBestSolutionInNeighborhoodN4(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, 0, true);

	//// Строим множество соседних решений по окрестности N5 от текущего решения и выбираем лучшее решение в окрестности
	//findBestSolutionInNeighborhoodN5(aCellsData, aMatDist, cellsNumb, aMatSolutionIteration, aMatAssignmentSolutionIteration, 0, true);
}

void removeInvalidCellsAssignment(int **aCellsData, int **aMatDist, int cellsNumb, int **aMatSolutionIteration, int **aMatAssignmentSolutionIteration) {

	int **aVolumsInContainer = new int*[2];
	for (int count = 0; count < 2; count++)
		aVolumsInContainer[count] = new int[cellsNumb];

	// из контейнеров где есть превышение объема удалим прикрепление ячеек в порядке возрастания объемов до тех пор пока ограничения на вместимость не будут выполняться
	for (int i = 0; i < cellsNumb; i++) {

		if (aMatSolutionIteration[i][1] > aCellsData[i][0]) {
			int numAssignedCells = 0;
			setValueIn2Array(aVolumsInContainer, 2, cellsNumb, 10000); // заполняем матрицу по умолчанию предельными значениям
			for (int j = 0; j < cellsNumb; j++) {
				if (aMatAssignmentSolutionIteration[i][j] == 1 && i != j) {
					aVolumsInContainer[0][j] = aCellsData[j][1];
					aVolumsInContainer[1][j] = j;
					numAssignedCells++;
				}
			}

			// если в контейнере есть остатки (они всегда назначаются сами в себя), то их тоже считаем, хотя в выборе на исключение они не участвуют
			if (aCellsData[i][1] > 0)
				numAssignedCells++;

			// очищаем стоимость размещение товаров в контейнер от величины штрафа, так как информация о нем больше не потребуется
			aMatSolutionIteration[i][2] -= aMatSolutionIteration[i][3];
			aMatSolutionIteration[i][3] = 0;

			// сортируем массив по возрастанию объемов
			insertionSort(aVolumsInContainer, cellsNumb, 1);
			for (int k = 0; k < cellsNumb; k++) {
				int j = aVolumsInContainer[1][k];
				aMatSolutionIteration[i][1] -= aCellsData[j][1];
				aMatSolutionIteration[i][2] -= aMatDist[i][j];
				aMatAssignmentSolutionIteration[i][j] = 0;
				numAssignedCells--;
				if (aMatSolutionIteration[i][1] <= aCellsData[i][0])
					break;
			}

			if (numAssignedCells == 0) {
				aMatSolutionIteration[i][0] = -1;
				aMatSolutionIteration[i][1] = -1;
				aMatSolutionIteration[i][2] = -1;
				aMatSolutionIteration[i][3] = -1;
			}
		}
	}

}

int getSolutionTotalCosts(int ** aMatSolution, int cellsNumb) {
	int costs = 0;
	for (int i = 0; i < cellsNumb; i++) {
		costs += aMatSolution[i][2];
	}
	return costs;
}