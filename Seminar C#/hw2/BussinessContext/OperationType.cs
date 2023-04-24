namespace HW02.BussinessContext;

/*
 * 0-1 = Add
 * 2-3 = Delete
 * 4-6 = Get
 */
public enum OperationType
{
    AddProduct = 0,
    AddCategory = 1,
    DeleteProduct = 2,
    DeleteCategory = 3,
    ListProducts = 4,
    ListCategories = 5,
    GetProductByCategory = 6
}