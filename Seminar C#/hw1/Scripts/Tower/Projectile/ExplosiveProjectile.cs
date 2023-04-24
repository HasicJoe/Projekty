
using UnityEngine;

public class ExplosiveProjectile : Projectile
{
    private const float _explosiveRadiusDistance = 5.0f; 
    protected override void OnTriggerEnter(Collider other)
    {
        base.OnTriggerEnter(other);
        Collider[] colliders = Physics.OverlapSphere(transform.position, _explosiveRadiusDistance);
        foreach (Collider collider in colliders)
        {
            if ((collider != other) && ((collider.gameObject.GetComponent<LazyEnemy>() != null)))
            {
                collider.gameObject.GetComponent<LazyEnemy>().ReduceHealth(Damage);
            }
            else if ((collider != other) && ((collider.gameObject.GetComponent<AggresiveEnemy>() != null)))
            {
                collider.gameObject.GetComponent<AggresiveEnemy>().ReduceHealth(Damage);
            }
        }
    }
}
