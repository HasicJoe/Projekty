using System;

namespace Festival.BL.Models
{
    public record ModelBase : IModel
    {
        public Guid Id { get; set; }
    }
}
