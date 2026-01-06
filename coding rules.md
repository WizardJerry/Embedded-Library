# 代码规范
## 命名规则
- 函数命名使用蛇形命名法，小写加下划线：`mp2797_i2c_init()`
- 变量命名使用小驼峰命名法，首字母小写，后续的每一个单词首字母大写：`countFlag`
- 结构体命名使用大驼峰+尾缀的方式，例如 `PidController_t`
- define的常量使用全大写加下划线命名：`GPIO_I2C_SDA`
- 命名过程中减少缩写，除非一些较为公认的名词如 `ADC, SPI, GPIO, PID`

## 文本规则
- 代码文件使用UTF-8编码

### 空白符的使用

参考代码：
``` C
float screen_decode_float(byte *data, float divisor) {
    uint32_t intValue = 0;
    for (int i = 0; i < 4; i++) {
        intValue |= (uint32_t)data[i] << (i * 8);
    }
    float floatValue = (float)intValue;
    return floatValue / divisor;
}
```

- 使用空格作为缩进，避免使用制表符

以4个空格作为缩进的长度

> 制表符在不同的编辑器中打开对应的长度不同，如果有空格和制表符混用的情况会导致代码缩进错乱。
> [Keil 中设置制表符输出空格](https://blog.csdn.net/MQ0522/article/details/130079300)

- `, ;` 等分隔符之后添加空格
- 运算赋值符号的两边添加空格
- 类型转换的括号和变量之间不添加空格
- 函数名和函数变量之间不添加空格


### 符号的使用

- 使用大括号 `{}` 括号时，左括号与函数定义或`if, while`等语句在同一行
> 参考代码可见 *空白符的使用*

- 用户定义的头文件使用引号引入 `#include "PID.h"`，系统自带的头文件使用尖括号来引入`#include <stdio.h>`

### 函数规则

- 函数内部不同段落之间使用一行回车分隔

- 函数与函数之间使用两行回车空行分隔

### 注释规则

- 变量或常量的注释与声明在同一行

```C
typedef struct {
    void* buffer;           // 队列缓冲区
    uint16_t capacity;      // 队列容量
    uint16_t size;          // 当前队列大小
    uint16_t head;          // 队列头指针
    uint16_t tail;          // 队列尾指针
    size_t element_size;    // 元素大小
    bool is_initialized;    // 初始化标志
} queue_t;
```


- 函数的注释放在 `.C` 文件中，放在函数上方，使用 [Doxygon](https://www.doxygen.nl/index.html) 风格的注释
``` C
/**
 * @brief  ADC start output voltage conversion
 * @param  startFlag: only start adc if flag = true
 * @note   B383 for FSR 4.096V, B183 for FSR 6.048V
 * @return None
 */
void adc_start_voltage(bool startFlag){
    if (startFlag){
        adc_write_reg(0xB383);
    }
}
```


## 程序建议

- 单个函数实现的有效代码通常不要超过 80 行，如果单个函数的代码过长，说明实现方式上大概率有优化空间

- 使用判断语句实现功能时，最好不要超过 3 层的嵌套，嵌套层数过多会带来代码阅读和理解上的困难，循环语句同理
