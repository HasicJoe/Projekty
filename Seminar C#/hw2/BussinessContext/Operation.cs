namespace HW02.BussinessContext;

public class Operation
{
    public readonly string operationName;
    public readonly string entityType;
    public readonly bool operationSuccess;
    public string exceptionInfo;
    public Entity entity;

    public Operation(OperationType opType, bool opSuccess, Entity e)
    {

        operationName = opType switch
        {
            OperationType.AddProduct =>  "Add",
            OperationType.AddCategory => "Add" ,
            OperationType.DeleteProduct => "Delete" ,
            OperationType.DeleteCategory => "Delete" ,
            OperationType.ListCategories => "Get",
            OperationType.ListProducts => "Get",
            OperationType.GetProductByCategory => "Get",
        };
        entityType = opType switch
        {
            OperationType.AddProduct => "Product",
            OperationType.DeleteProduct => "Product",
            OperationType.ListProducts => "Product",
            OperationType.GetProductByCategory => "Product",
            OperationType.AddCategory => "Category",
            OperationType.DeleteCategory => "Category",
            OperationType.ListCategories => "Category",
        };
        operationSuccess = opSuccess;
        entity = e;
    }

    public void AddExceptionInfo(string exMsg)
    {
        if (!operationSuccess)
        {
            exceptionInfo = exMsg;
        }
    }
}