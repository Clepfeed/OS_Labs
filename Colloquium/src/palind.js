/**
 * Проверяет, является ли число палиндромом
 * @param {number} num - проверяемое число
 * @returns {boolean} - true, если число является палиндромом, иначе false
 * @throws {Error} - если входной параметр не является числом
 */
function isPalindrome(num) {
    // Проверка входных данных
    if (typeof num !== 'number') {
      throw new Error('Входной параметр должен быть числом');
    }
    
    if (!Number.isFinite(num)) {
      throw new Error('Входной параметр должен быть конечным числом');
    }
    
    // Обработка отрицательных чисел (они не могут быть палиндромами из-за минуса в начале)
    if (num < 0) {
      return false;
    }
    
    // Обработка десятичных чисел (преобразуем к строке и проверяем как строку)
    const numStr = num.toString();
    
    // Реализация проверки палиндрома через сравнение символов с обоих концов
    const length = numStr.length;
    for (let i = 0; i < Math.floor(length / 2); i++) {
      if (numStr[i] !== numStr[length - 1 - i]) {
        return false;
      }
    }
    
    return true;
  }
  
  // Юнит-тесты
  class PalindromeTest {
    static runTests() {
      this.testInvalidInputs();
      this.testNegativeNumbers();
      this.testZero();
      this.testSingleDigit();
      this.testPalindromes();
      this.testNonPalindromes();
      this.testDecimalNumbers();
      console.log('Все тесты функции isPalindrome пройдены успешно!');
    }
  
    static testInvalidInputs() {
      const testCases = [
        { input: 'string', expectedError: 'Входной параметр должен быть числом' },
        { input: null, expectedError: 'Входной параметр должен быть числом' },
        { input: undefined, expectedError: 'Входной параметр должен быть числом' },
        { input: NaN, expectedError: 'Входной параметр должен быть конечным числом' },
        { input: Infinity, expectedError: 'Входной параметр должен быть конечным числом' }
      ];
  
      testCases.forEach(({ input, expectedError }) => {
        try {
          isPalindrome(input);
          throw new Error(`Тест не пройден: исключение не было выброшено для входа ${input}`);
        } catch (error) {
          if (error.message !== expectedError) {
            throw new Error(`Тест не пройден: для входа ${input} ожидалась ошибка "${expectedError}", но получена "${error.message}"`);
          }
        }
      });
    }
  
    static testNegativeNumbers() {
      const testCases = [
        { input: -1, expected: false },
        { input: -121, expected: false },
        { input: -12321, expected: false }
      ];
  
      testCases.forEach(({ input, expected }) => {
        const result = isPalindrome(input);
        if (result !== expected) {
          throw new Error(`Тест не пройден: для числа ${input} ожидался результат ${expected}, но получен ${result}`);
        }
      });
    }
  
    static testZero() {
      const result = isPalindrome(0);
      if (result !== true) {
        throw new Error(`Тест не пройден: ноль должен быть палиндромом`);
      }
    }
  
    static testSingleDigit() {
      const testCases = [1, 2, 3, 4, 5, 6, 7, 8, 9];
  
      testCases.forEach(input => {
        const result = isPalindrome(input);
        if (result !== true) {
          throw new Error(`Тест не пройден: для числа ${input} ожидался результат true, но получен ${result}`);
        }
      });
    }
  
    static testPalindromes() {
      const testCases = [11, 121, 12321, 1234321, 123454321];
  
      testCases.forEach(input => {
        const result = isPalindrome(input);
        if (result !== true) {
          throw new Error(`Тест не пройден: число ${input} должно быть палиндромом`);
        }
      });
    }
  
    static testNonPalindromes() {
      const testCases = [12, 123, 1232, 12345, 123456];
  
      testCases.forEach(input => {
        const result = isPalindrome(input);
        if (result !== false) {
          throw new Error(`Тест не пройден: число ${input} не должно быть палиндромом`);
        }
      });
    }
  
    static testDecimalNumbers() {
      const testCases = [
        { input: 1.1, expected: true },
        { input: 12.21, expected: true },
        { input: 1.2, expected: false },
        { input: 12.3, expected: false }
      ];
  
      testCases.forEach(({ input, expected }) => {
        const result = isPalindrome(input);
        if (result !== expected) {
          throw new Error(`Тест не пройден: для числа ${input} ожидался результат ${expected}, но получен ${result}`);
        }
      });
    }
  }
  
  // Запуск тестов
  // PalindromeTest.runTests();