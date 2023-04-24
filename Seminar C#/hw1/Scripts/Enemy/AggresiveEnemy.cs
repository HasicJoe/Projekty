using System.Collections.Generic;
using UnityEngine;

public class AggresiveEnemy : Enemy
{
    protected const float _minimalRadiusToTower = 10.0f;
    protected GameObject _selectedTower;
    protected bool _hasTower = false;

    private void Update()
    {
        if (_selectedTower == null && _hasTower == false)
        {
            GameObject TowerInRadius = FindTowerInRadius();
            if (TowerInRadius != null)
            {
                //do not have to call every frame
                _selectedTower = TowerInRadius;
                _movementComponent.MoveTowards(_selectedTower.transform);
                _hasTower = true;
            }
        }
        else if (_selectedTower == null && _hasTower == true)
        {
            // tower has been destroyed
            _movementComponent.MoveAlongPath();
            _hasTower = false;
        }
    }


    private GameObject FindTowerInRadius()
    {
        Collider[] colliders = Physics.OverlapSphere(transform.position, _minimalRadiusToTower);
        foreach (Collider collider in colliders)
        {
            if (collider.gameObject.GetComponent<BasicTower>() != null ||
                collider.gameObject.GetComponent<BurstTower>() != null ||
                collider.gameObject.GetComponent<RandomTower>() != null)
            {
                return collider.gameObject;
            }

        }
        return null;
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (collision.gameObject.GetComponent<BasicTower>() != null)
        {
            collision.gameObject.GetComponent<BasicTower>().Health.HealthValue -= Damage;
        }
        else if (collision.gameObject.GetComponent<BurstTower>() != null)
        {
            collision.gameObject.GetComponent<BurstTower>().Health.HealthValue -= Damage;
        }
        else if (collision.gameObject.GetComponent<RandomTower>() != null)
        {
            collision.gameObject.GetComponent<RandomTower>().Health.HealthValue -= Damage;
        }
        else if (collision.gameObject.GetComponent<Castle>() != null)
        {
            collision.gameObject.GetComponent<Castle>().Health.HealthValue -= Damage;
        }
    }
}
