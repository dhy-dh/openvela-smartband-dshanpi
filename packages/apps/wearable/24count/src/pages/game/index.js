const operations = {
  '+': (a, b) => a + b,
  '-': (a, b) => a - b,
  '*': (a, b) => a * b,
  '/': (a, b) => a / b,
};
const operateIndex = {
  '+': 4,
  '-': 5,
  '*': 6,
  '/': 7,
}
import app from "@system.app";
import folme from "@system.folme"
import storage from '@system.storage'
import { primaryLevel, middleAndHignLevel } from '../../common/utils'
export default {
  data: {
    typeLevel: 10,//难易级别
    currentClick: '',//当前鼠标位置
    currentNumber: [],//当前的随机四个数
    restartArr: [],//用于重新开始存储的数据
    temptureArr: [],//记录临时点击的两个数字信息
    hideNumber: [],//计算之后隐藏的数字项
    acType: '',//计算类型
    passCurret: false,
    // borderClick: [],
    countLevel: 1,
    clickCount: 0,
  },
  onInit() {
    this.initGame()
  },
  initGame() {
    let result=this.arrayShuffle(this.typeLevel == 10 ? primaryLevel : middleAndHignLevel)
    const randomNumber = Math.floor(Math.random() * 111) + 1;
    // let randomArr = this.generateRandomNumbers(this.typeLevel)
    let randomArr=result[randomNumber]
    this.currentNumber = randomArr;
    storage.set({
      key: 'restartArr',
      value: JSON.stringify(randomArr)
    })
  },
  resetState() {
    this.hideNumber = [];
    this.clickCount = 0;
    this.temptureArr = [];
    this.acType = ''
  },
  nextLevel() {
    this.resetState();
    this.countLevel++;
    this.passCurret = false;
    this.initGame()
  },
  async reStart() {
    let _that = this;
    folme.to({
      id: "restarticon",
      toState: {
        rotate: 360,
      },
      config: {
        ease: ['quadInOut', 0.4]
      }
    });
    await storage.get({
      key: 'restartArr',
      success: function (data) {
        _that.currentNumber = JSON.parse(data);
      },
    })
    this.currentClick = ""
    this.passCurret = false;
    this.resetState()
  },
  exit() {
    app.terminate();
  },
  //加减乘动作
  actionType(type) {
    this.currentClick = operateIndex[type]
    if (this.temptureArr.length == 0) return
    // const allKeys = this.temptureArr.flatMap(obj => Object.keys(obj));
    // this.borderClick[1] = Number(allKeys[0])
    this.acType = type
  },
  //数字点击
  operatenumber(currentClick) {
    try {
    this.currentClick = currentClick
    if (!this.acType) {
      this.temptureArr[0] = {
        [currentClick]: this.currentNumber[currentClick]
      }
      // this.borderClick[0] = Number(allKeys[1])
      return
    }
    if (this.temptureArr.length == 1) {
      this.temptureArr.push({
        [currentClick]: this.currentNumber[currentClick]
      })
      //点击的第一个与第二个索引
      const allKeys = this.temptureArr.flatMap(obj => Object.keys(obj));
      if (allKeys[0] == allKeys[1]) {
        this.temptureArr.splice(1, 1)
        this.acType=''
        return 
      }
      let firstValue = this.temptureArr[0][allKeys[0]]
      let secondValue = this.temptureArr[1][allKeys[1]]
      if (this.acType == '/') {
        if (firstValue % secondValue == 0) {
          this.operateState(allKeys, firstValue, secondValue)
        } else {
          this.temptureArr.splice(1, 1)
          this.acType = ''
          return
        }
      } else {
        this.operateState(allKeys, firstValue, secondValue)
      }
    }
    } catch (error) {
      console.log(error,'catch error')
    }
  },
  //修改页面点击，计算后的隐藏，边框，圆圈展示
  operateState(allKeys, firstValue, secondValue) {
    let results = operations[this.acType]?.(firstValue, secondValue);
    this.currentNumber[allKeys[1]] = results;
    this.temptureArr[1][allKeys[1]] = results;
    this.hideNumber.push(Number(allKeys[0]));
    this.temptureArr.splice(0, 1);
    // this.borderClick[0] = Number(allKeys[1])
    this.acType = '';
    this.currentClick = '';
    this.clickCount++;
    this.checkState(results)
  },
  checkState(results) {
    if (this.clickCount == 3 && results==24)
      this.passCurret = true;
  },
  //随机生成4个数
  generateRandomNumbers(typeLevel) {
    let numbers;
    do {
      numbers = Array.from({ length: 4 }, () => Math.floor(Math.random() * typeLevel) + 1);
    } while (!this.judgePoint24(numbers));
    return numbers;
  },
  //校验随机4个数字
  judgePoint24(nums) {
    if (nums.length == 1)
      return Math.abs(nums[0] - 24) < 1e-6;
    for (let i = 0; i < nums.length; i++)
      for (let j = i + 1; j < nums.length; j++) {
        let rest = nums.filter((value, index) => index != i && index != j);
        if (this.judgePoint24([nums[i] + nums[j], ...rest]) || this.judgePoint24([nums[i] * nums[j], ...rest]) ||
          this.judgePoint24([nums[i] - nums[j], ...rest]) || this.judgePoint24([nums[j] - nums[i], ...rest]) ||
          this.judgePoint24([nums[i] / nums[j], ...rest]) || this.judgePoint24([nums[j] / nums[i], ...rest]))
          return true;
      }
    return false;
  },
  deepClone(obj) {
    if (typeof obj !== 'object' || obj === null) {
      return obj;
    }

    const clone = Array.isArray(obj) ? [] : {};

    for (let key in obj) {
      if (obj.hasOwnProperty(key)) {
        clone[key] = this.deepClone(obj[key]);
      }
    }

    return clone;
  },
  arrayShuffle(arr) {
    return arr
      .map((item) => ({
        key: Math.random(),
        value: item,
      }))
      .sort((a, b) => a.key - b.key)
      .map((item) => item.value);
  },
   smallSize(num) {
    return num.toString().length > 4?'30px':num.toString().length > 3?'40px':'50px';
  }
}