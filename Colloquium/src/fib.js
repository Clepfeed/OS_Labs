/**
 * Генерирует контейнер с первыми n числами Фибоначчи
 * @param {number} n - количество чисел Фибоначчи (натуральное число)
 * @returns {Array<number>} - массив с первыми n числами Фибоначчи
 * @throws {Error} - если n не является натуральным числом
 */
function generateFibonacci(n) {
    // Проверка входных данных
    if (typeof n !== 'number') {
      throw new Error('Входной параметр должен быть числом');
    }
    
    if (!Number.isInteger(n)) {
      throw new Error('Входной параметр должен быть целым числом');
    }
    
    if (n <= 0) {
      throw new Error('Входной параметр должен быть натуральным числом (> 0)');
    }
    
    // Обработка специальных случаев
    if (n === 1) {
      return [0];
    }
    
    if (n === 2) {
      return [0, 1];
    }
    
    // Основной алгоритм для n >= 3
    const fibonacci = [0, 1];
    
    for (let i = 2; i < n; i++) {
      // Для больших чисел может возникнуть переполнение,
      // но JavaScript автоматически перейдет к BigInt при необходимости
      fibonacci[i] = fibonacci[i - 1] + fibonacci[i - 2];
      
      // Для очень больших n можно добавить проверку на бесконечность
      if (!isFinite(fibonacci[i])) {
        throw new Error('Переполнение: результат слишком большой для представления в JavaScript');
      }
    }
    
    return fibonacci;
  }
  
  // Юнит-тесты
  class FibonacciTest {
    static runTests() {
      this.testInvalidInputs();
      this.testSpecialCases();
      this.testRegularCases();
      this.testLargeNumbers();
      console.log('Все тесты функции generateFibonacci пройдены успешно!');
    }
  
    static testInvalidInputs() {
      const testCases = [
        { input: 'string', expectedError: 'Входной параметр должен быть числом' },
        { input: null, expectedError: 'Входной параметр должен быть числом' },
        { input: undefined, expectedError: 'Входной параметр должен быть числом' },
        { input: 1.5, expectedError: 'Входной параметр должен быть целым числом' },
        { input: 0, expectedError: 'Входной параметр должен быть натуральным числом (> 0)' },
        { input: -5, expectedError: 'Входной параметр должен быть натуральным числом (> 0)' }
      ];
  
      testCases.forEach(({ input, expectedError }) => {
        try {
          generateFibonacci(input);
          throw new Error(`Тест не пройден: исключение не было выброшено для входа ${input}`);
        } catch (error) {
          if (error.message !== expectedError) {
            throw new Error(`Тест не пройден: для входа ${input} ожидалась ошибка "${expectedError}", но получена "${error.message}"`);
          }
        }
      });
    }
  
    static testSpecialCases() {
      const testCases = [
        { input: 1, expected: [0] },
        { input: 2, expected: [0, 1] }
      ];
  
      testCases.forEach(({ input, expected }) => {
        const result = generateFibonacci(input);
        const isEqual = JSON.stringify(result) === JSON.stringify(expected);
        
        if (!isEqual) {
          throw new Error(`Тест не пройден: для n=${input} ожидался результат ${expected}, но получен ${result}`);
        }
      });
    }
  
    static testRegularCases() {
      const testCases = [
        { input: 5, expected: [0, 1, 1, 2, 3] },
        { input: 8, expected: [0, 1, 1, 2, 3, 5, 8, 13] },
        { input: 10, expected: [0, 1, 1, 2, 3, 5, 8, 13, 21, 34] }
      ];
  
      testCases.forEach(({ input, expected }) => {
        const result = generateFibonacci(input);
        const isEqual = JSON.stringify(result) === JSON.stringify(expected);
        
        if (!isEqual) {
          throw new Error(`Тест не пройден: для n=${input} ожидался результат ${expected}, но получен ${result}`);
        }
      });
    }
  
    static testLargeNumbers() {
      // Проверяем, что функция корректно обрабатывает большие числа
      const n = 50;
      const result = generateFibonacci(n);
      
      // Проверяем определенные свойства последовательности Фибоначчи
      if (result.length !== n) {
        throw new Error(`Тест не пройден: длина результата должна быть ${n}, но получена ${result.length}`);
      }
      
      // Проверяем рекуррентное соотношение для нескольких последних элементов
      for (let i = 2; i < result.length; i++) {
        if (result[i] !== result[i-1] + result[i-2]) {
          throw new Error(`Тест не пройден: нарушено рекуррентное соотношение для i=${i}`);
        }
      }
    }
  }
  
  // Запуск тестов
  // FibonacciTest.runTests();