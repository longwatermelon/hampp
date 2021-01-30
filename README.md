# hampp

ham written in cpp

If you want to build the project yourself, download simple-json-lib from [here](https://github.com/longwatermelon/simple-json-lib) and then use

```
g++ -I src -I (directory jsonlib.h is in) src/*.cpp -o build/hampp -std=c++17
```

# i am aware that struct members must all be named uniquely regardless of if they are members of different structs or not.

i just didnt want to refactor my code again to fix this

example

```
strooct foo
{
  def name = "foo";
};

strooct bar
{
  def name = "bar";
};

def foo = new foo;
def bar = new bar;
prount(foo.name, bar.name);
```

prints `foo foo`
