### Plugin Technical Documentation: Global Delegate System for Unreal Engine

#### Overview  
The Global Delegate System plugin provides a convenient mechanism for event management in Unreal Engine 5 projects. It allows easy event subscription, invocation, and unsubscription using macros to simplify the syntax.

---

### Core Components  

#### Main Macros  
- **GLOBAL_EVENT_LISTEN(Context, Namespace, EventName, Func):** Subscribe to an event.  
- **GLOBAL_EVENT_COMMIT(Namespace, EventName):** Invoke an event without arguments.  
- **GLOBAL_EVENT_COMMIT_ARGS(Namespace, EventName, ...):** Invoke an event with arguments.  
- **GLOBAL_EVENT_REMOVE_LISTEN(Context):** Unsubscribe from an event.  

#### **FGlobalEventDispatcherHandler Class**  
The primary class for event management.

**Main Methods:**  
- `AddListen(ClassType* Context, const FName& Namespace, const FName& EventName, void (ClassType::* Func)(Args...))`: Adds an event subscription.  
- `Commit(const FName& Namespace, const FName& EventName, Args... args)`: Invokes an event with arguments.  
- `RemoveListen(const void* Context)`: Removes all subscriptions for the specified context.  

---

### Helper Structures and Classes  

#### **IsPointer, AllAreNotPointer**  
Template structures for checking whether all arguments are non-pointer types.

#### **FBaseVariant, FVariantImpl, FUniversalVariant**  
Classes for storing various data types in a universal container.

#### **FDelegateKey**  
A structure for storing delegate keys used in `DelegateMap`.

#### **IGlobalDelegate**  
An interface for delegates.

#### **TGlobalDelegate**  
A templated delegate class that stores a pointer to an object and a member function.

---

### Class and Method Descriptions  

#### **FGlobalEventDispatcherHandler Class**  
```cpp
class GLOBALEVENTDISPATCHER_API FGlobalEventDispatcherHandler
{
public:
    FGlobalEventDispatcherHandler();
    ~FGlobalEventDispatcherHandler();

    static TSharedPtr<FGlobalEventDispatcherHandler> Get();

    template<typename ClassType, typename... Args>
    void AddListen(ClassType* Context, const FName& Namespace, const FName& EventName, void (ClassType::* Func)(Args...));

    template<typename... Args>
    void Commit(const FName& Namespace, const FName& EventName, Args... args);

    void RemoveListen(const void* Context);

private:
    TMap<FDelegateKey, TArray<TSharedPtr<IGlobalDelegate>>> DelegateMap;
};
```
##### **AddListen**  
Adds an event subscription.  
- **Context**: Pointer to an object.  
- **Namespace**: Event namespace.  
- **EventName**: Event name.  
- **Func**: Pointer to a member function.  

##### **Commit**  
Invokes an event with specified arguments.  
- **Namespace**: Event namespace.  
- **EventName**: Event name.  
- **args**: Event arguments.  

##### **RemoveListen**  
Removes all subscriptions for the specified context.  
- **Context**: Pointer to the object whose subscriptions need to be removed.  

---

### Helper Classes and Structures  

#### **FBaseVariant**  
```cpp
class FBaseVariant
{
public:
    virtual ~FBaseVariant() = default;
};
```
#### **FVariantImpl**  
```cpp
template<typename T>
class FVariantImpl : public FBaseVariant
{
public:
    FVariantImpl() {}
    FVariantImpl(T& InValue) : Value(InValue) {}

    T& GetValue() const;

private:
    T Value;
};
```
#### **FUniversalVariant**  
```cpp
class FUniversalVariant
{
public:
    template<typename T>
    FUniversalVariant(T& InValue);

    template<typename T>
    T Get() const;

private:
    TSharedPtr<FBaseVariant> Data;
};
```
#### **FDelegateKey**  
```cpp
struct FDelegateKey
{
    FName Namespace;
    FName EventName;

    bool operator==(const FDelegateKey& Other) const;

    friend uint32 GetTypeHash(const FDelegateKey& Key);
};
```
#### **IGlobalDelegate**  
```cpp
class IGlobalDelegate
{
public:
    virtual ~IGlobalDelegate() = default;
    virtual void* GetOwner() const = 0;
    virtual void Execute(const TArray<FUniversalVariant>& Params) = 0;
};
```
#### **TGlobalDelegate**  
```cpp
template<typename TOwner, typename... ParamTypes>
class TGlobalDelegate : public IGlobalDelegate
{
public:
    TGlobalDelegate(TOwner* Object, void(TOwner::* InFunction)(ParamTypes...));

    virtual void* GetOwner() const override;
    virtual void Execute(const TArray<FUniversalVariant>& Params) override;

private:
    TOwner* OwnerRef;
    void (TOwner::* Function)(ParamTypes...);

    template<int... Indices>
    void CallFunction(const TArray<FUniversalVariant>& Params, TIntegerSequence<int, Indices...>);

    template<typename T>
    T GetParam(const FUniversalVariant& Variant) const;
};
```
---

### Usage Examples  

#### **Subscribing to an Event**  
```cpp
GLOBAL_EVENT_LISTEN(this, "MyNamespace", "MyEvent", &MyClass::MyFunction);
```
#### **Invoking an Event**  
```cpp
GLOBAL_EVENT_COMMIT("MyNamespace", "MyEvent");
```
#### **Invoking an Event with Arguments**  
```cpp
GLOBAL_EVENT_COMMIT_ARGS("MyNamespace", "MyEvent", Arg1, Arg2);
```
#### **Removing an Event Subscription**  
```cpp
GLOBAL_EVENT_REMOVE_LISTEN(this);
```
