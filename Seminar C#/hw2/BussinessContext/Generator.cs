namespace HW02.BussinessContext;

public class Generator
{
    private int _categoryGenerator = 1;
    private int _productGenerator = 1;

    public int GenerateCategoryId()
    {
        return _categoryGenerator++;
    }

    public int GenerateProductId()
    {
        return _productGenerator++;
    }

}