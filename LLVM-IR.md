# LLVM 语言手册
---
## 摘要
该文档是`LLVM`汇编语言参考手册。`LLVM`是静态单赋值(SSA)形式的中间语言。并提供了类型安全，低级操作，灵活性，并可以表达几乎所有的高级语言的语义。LLVM语言是在所有编译过程中最常见的表示形式。

## 简介
LLVM中间代码被设计在多种形式下使用。
- 传统编译器的中间代码
- 作为*字节码*保存于磁盘（并可以用*JIT*编译器快速加载）
- 使用者可读的汇编语言形式

这使LLVM在编译器的转换与分析中成为一种强大的中间表示形式。这三种使用LLVM的方式是完全相同的。本文档描述了可读表示。

LLVM的目标为保持轻量且低级的同时具有强大的可读性、类型信息，及可扩展性。通过为所有的高级思想都可以对应到足够低的层次而成为一种“通用IR”。
提供类型信息，LLVM可以做为优化目标，例如：可以通过证明C语言中的某指针从不在当前过程外读取，从而使用一个简单的SSA临时变量表示，而非为其分配内存

## 良构
值得注意的是，本文档描述了一个**良构**的LLVM汇编语言。可被编译器**接受**与**良构**是有区别的。例如以下代码一直是可行的，但并不是良构的:

```llvm
%x = add i32 1, %x
```
because the definition of %x does not dominate all of its uses. The LLVM infrastructure provides a verification pass that may be used to verify that an LLVM module is well formed. This pass is automatically run by the parser after parsing input assembly and by the optimizer before it outputs bitcode. The violations pointed out by the verifier pass indicate bugs in transformation passes or input to the parser.

## 标识符
LLVM标识符有两种基本形式：全局与局部。全局标识符（函数与全局变量）由 `@`字符开头。局部标识符（临时变量、类型）由`%`开头。此外，对于不同的目的，有3种不同形式的标识符格式：
1. 具名值由一个带前缀的字符串表示。例如，*%foo*, *%@DivisionByZero*, *%a.really.long.identifier*。该字符串应满足`[%@][-a-zA-Z$._][-a-zA-Z$._0-9]*`。依赖其他字符的标识符应该被引号包围。ASCII特殊字符可由"\xx"表示，其中xx是该字符的ASCII编码的十六进制表示。通过这种方式，任意字符可以被用作具名值，甚至包括引号自身。全局标识符可"\01"前缀来避免被重新命名。
2. 非具名值可用无符号值及其前缀表示。例如 *%12*, *@2*, *%44*。
3. 常量。在后续的常量章节描述。

简单方法
```llvm
%result = mul i32 %X, 8
```
优化
```llvm
%result = shl i32 %x, 3
```
地狱难度
```llvm
%0      = add i32 %X, %X           ; yields i32:%0
%1      = add i32 %0, %0           ; yields i32:%1
%result = add i32 %1, %1
```
