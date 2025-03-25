/**
 * Класс для представления узла связного списка
 */
class ListNode {
    /**
     * Создает новый узел связного списка
     * @param {*} value - значение, хранимое в узле
     * @param {ListNode|null} next - ссылка на следующий узел (или null, если это последний узел)
     */
    constructor(value, next = null) {
      this.value = value;
      this.next = next;
    }
  }
  
  /**
   * Разворачивает связный список итеративным способом
   * @param {ListNode|null} head - головной узел связного списка
   * @returns {ListNode|null} - новый головной узел развернутого списка
   */
  function reverseLinkedList(head) {
    // Обработка специальных случаев
    if (head === null) {
      return null; // Пустой список
    }
    
    if (head.next === null) {
      return head; // Список из одного элемента
    }
    
    let prev = null;
    let current = head;
    let next = null;
    
    // Итеративный проход по списку
    while (current !== null) {
      // Сохраняем ссылку на следующий узел
      next = current.next;
      
      // Меняем ссылку текущего узла на предыдущий
      current.next = prev;
      
      // Перемещаем указатели на следующую итерацию
      prev = current;
      current = next;
    }
    
    // prev теперь указывает на новую голову списка
    return prev;
  }
  
  /**
   * Вспомогательная функция для создания связного списка из массива
   * @param {Array} arr - массив значений для создания списка
   * @returns {ListNode|null} - головной узел созданного связного списка
   */
  function createLinkedList(arr) {
    if (!arr || arr.length === 0) {
      return null;
    }
    
    const head = new ListNode(arr[0]);
    let current = head;
    
    for (let i = 1; i < arr.length; i++) {
      current.next = new ListNode(arr[i]);
      current = current.next;
    }
    
    return head;
  }
  
  /**
   * Вспомогательная функция для преобразования связного списка в массив
   * @param {ListNode|null} head - головной узел связного списка
   * @returns {Array} - массив значений из связного списка
   */
  function linkedListToArray(head) {
    const result = [];
    let current = head;
    
    while (current !== null) {
      result.push(current.value);
      current = current.next;
    }
    
    return result;
  }
  
  // Юнит-тесты
  class ReverseLinkedListTest {
    static runTests() {
      this.testEmptyList();
      this.testSingleNodeList();
      this.testTwoNodeList();
      this.testMultiNodeList();
      this.testLargeList();
      console.log('Все тесты функции reverseLinkedList пройдены успешно!');
    }
  
    static testEmptyList() {
      const result = reverseLinkedList(null);
      if (result !== null) {
        throw new Error('Тест не пройден: для пустого списка ожидался результат null');
      }
    }
  
    static testSingleNodeList() {
      const list = new ListNode(5);
      const result = reverseLinkedList(list);
      
      if (result.value !== 5 || result.next !== null) {
        throw new Error('Тест не пройден: список из одного элемента должен остаться неизменным');
      }
    }
  
    static testTwoNodeList() {
      const list = createLinkedList([1, 2]);
      const result = reverseLinkedList(list);
      const resultArray = linkedListToArray(result);
      
      if (JSON.stringify(resultArray) !== JSON.stringify([2, 1])) {
        throw new Error(`Тест не пройден: для списка [1, 2] ожидался результат [2, 1], но получен ${JSON.stringify(resultArray)}`);
      }
    }
  
    static testMultiNodeList() {
      const testCases = [
        { input: [1, 2, 3, 4, 5], expected: [5, 4, 3, 2, 1] },
        { input: [10, 20, 30], expected: [30, 20, 10] }
      ];
  
      testCases.forEach(({ input, expected }) => {
        const list = createLinkedList(input);
        const result = reverseLinkedList(list);
        const resultArray = linkedListToArray(result);
        
        if (JSON.stringify(resultArray) !== JSON.stringify(expected)) {
          throw new Error(`Тест не пройден: для списка ${JSON.stringify(input)} ожидался результат ${JSON.stringify(expected)}, но получен ${JSON.stringify(resultArray)}`);
        }
      });
    }
  
    static testLargeList() {
      // Создаем большой список
      const input = Array.from({ length: 1000 }, (_, i) => i + 1);
      const expected = [...input].reverse();
      
      const list = createLinkedList(input);
      const result = reverseLinkedList(list);
      const resultArray = linkedListToArray(result);
      
      if (JSON.stringify(resultArray) !== JSON.stringify(expected)) {
        throw new Error('Тест не пройден: большой список не был корректно развернут');
      }
    }
  }
  
  // Запуск тестов
  // ReverseLinkedListTest.runTests();