# C/C++ coding style

## line width
Keep your line between 80 to 120 characters long. It will easily fit in most monitors

## indentation
Use 4 spaces of indentation and max 3 nesting depth

```c
void fn() {
    if (/*...*/) { // 1
        if (/*...*/) { // 2
            if (/*...*/) { // 3
                // not okay
                if (/*...*/);
```

Having too much nesting can lead to high cognitive complexity and makes the code worse readable

## Braces

### If, else, else if
For oneliners you can do either

```c
if (cond) func();
```

or

```c
if (cond)
    func();
```

When the function and args are large, use the 2nd style

When your if statement has more than one line

```c
if (cond) {
   // code
} else {
   // code
}
```

if your else of else if is oneliner, you can just use previous style<br>

```c
if (cond) {
   // code
} else func();
```

### Functions and methods 
```c
int foo(int baz)
{
    return baz * 2;
}
```

### Structs and Classes
```c
typedef struct /*...*/ {
    int val;
} Foo;
```
```cpp
class Foo {
    public:
        int v;
    private:
        float c;
    /*...*/
}
```

### Switch
```c
switch (val) {
    case 'a':
    case 'A':
        // code
        break;
}
```

> Notice, variables made in `case` statement can be access in other case statemenets. In order to get round this issue

```c
switch (val) {
    case 'A': {
        // code
        break;
    }
    case 'a': {
        // code
        break;
    }
}
```

### Namespace

```c
namespace foo {
    /*...*/
}
```

## Headers

Every header contains declarations, in order to not over-declarate, use header guards

```h
#ifndef FOO_H
#define FOO_H
// declarations
#endif
```

## Variable naming style

When naming classes, structs and methods. Use PascalCase style 

```cpp
class Foo {
    public:
        Get(/*...*/);
};
// ...

typedef struct _Node {
    // ...
} Node;
```

Variable names should be camelCase.

```c
int helloWorld = 77;
```

for namespaces use snake_case.

## Pointers and references

In both function args and variables

```c
void foo(int& ref);
void bar(int* ptr);
// ...
{
    int* ptr = new int(0);
    int& ref = /*...*/;
}
```

When using pointers as function argumens and not changing their values, use `const`

## Stack vs Heap

Stack is fast and thus preffered for variables, objects created in the stack are automatically deallocated when out of scope.
Sometimes you must use the Heap so before creating any pointer, create smart pointer such as `std::unique_ptr` or `std::shared_ptr`

When moving smart pointers, use `std::move`

Why smart pointers? to have automatic memory management
