
using UnityEngine;

public class LazyEnemy : Enemy
{
    protected float ActiveTimer = 0.0f;
    protected float PassiveTimer = 0.0f;
    protected bool ActiveMode = true;
    protected const float ActiveTreshold = 5.0f;
    protected const float PassiveTreshold = 1.0f;
    protected const int CastleDamage = 25;


    private void ManageActiveState()
    {
        ActiveTimer += Time.deltaTime;
        if (ActiveTimer >= ActiveTreshold)
        {
            ActiveMode = false;
            ActiveTimer = 0.0f;
            _movementComponent.CancelMovement();
        }
    }

    private void ManagePassiveState()
    {
        PassiveTimer += Time.deltaTime;
        if (PassiveTimer >= PassiveTreshold)
        {
            ActiveMode = true;
            PassiveTimer = 0.0f;
            _movementComponent.MoveAlongPath();
        }
    }

    private void Update()
    {
        if (ActiveMode)
        {
            ManageActiveState();
        }
        else
        {
            ManagePassiveState();
        }
    }

    private void OnCollisionEnter(Collision collision)
    {
        if (!ActiveMode)
        {
            return;
        }
        if (collision.collider.gameObject.GetComponent<Castle>() != null)
        {
            collision.collider.gameObject.GetComponent<Castle>().Health.HealthValue -= CastleDamage;
        }
        if (collision.collider.gameObject.GetComponent<BasicTower>() != null)
        {
            collision.collider.gameObject.GetComponent<BasicTower>().Health.HealthValue -= 2 * CastleDamage;
        }
        if (collision.collider.gameObject.GetComponent<BurstTower>() != null)
        {
            collision.collider.gameObject.GetComponent<BurstTower>().Health.HealthValue -= 2 * CastleDamage;
        }
        if (collision.collider.gameObject.GetComponent<RandomTower>() != null)
        {
            collision.collider.gameObject.GetComponent<RandomTower>().Health.HealthValue -= 2 * CastleDamage;
        }
    }
}
