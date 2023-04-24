using System.Data;
using HW02.AnalyticalDataContext.DB;

namespace HW02.AnalyticalDataContext
{
    public class AnalyticalDataListener
    {
        public List<AnalyticalDataModel> dataModel = new List<AnalyticalDataModel>();
        private AnalyticalDBContext _analyticalDbContext = new AnalyticalDBContext();
        
        
        public AnalyticalDataListener(OperationManager om)
        {
            om.entityOperationHandler += AddData;
        }

    

        public void AddData(object sender, OperationEventArgs args)
        {
            if (args.Operation.operationSuccess)
            {
                if ((args.Operation.operationName == "Add") || (args.Operation.operationName == "Delete"))
                {
                    if (args.Operation.entity != null)
                    {
                        AnalyticalDataModel item =
                            dataModel.FirstOrDefault(i => i.CategoryId == args.Operation.entity.CategoryId);
                        if (item != null)
                        {
                            if (args.Operation.operationName == "Add")
                            {
                                item.ProductCount += 1;
                            }
                            else
                            {
                                if (args.Operation.entityType == "Category")
                                {
                                    dataModel.Remove(item);
                                }
                                else
                                {
                                    if (item.ProductCount <= 1)
                                    {
                                        dataModel.Remove(item);
                                    }
                                    else
                                    {
                                        item.ProductCount = item.ProductCount - 1;
                                    }
                                }
                            }
                        }
                        else
                        {
                            // add new category
                            AnalyticalDataModel new_item = new()
                            {
                                CategoryId = args.Operation.entity.CategoryId,
                                CategoryName = args.Operation.entity.Name,
                                ProductCount = 0,
                            };
                            dataModel.Add(new_item);
                        }
                    }
                }
            }

            _analyticalDbContext.SaveAnalyticalData(dataModel);
        }

        // TODO: implement the listener here
    }
}
