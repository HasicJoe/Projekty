using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(MovementComponent), typeof(HealthComponent), typeof(BoxCollider))]
public class Enemy : MonoBehaviour
{
    [SerializeField] protected MovementComponent _movementComponent;
    [SerializeField] protected HealthComponent _healthComponent;
    [SerializeField] protected ParticleSystem _onDeathParticlePrefab;
    [SerializeField] protected ParticleSystem _onSuccessParticlePrefab;
    [SerializeField] protected LayerMask _attackLayerMask;

    public event Action OnDeath;
    public int NumberOfLifes;
    public int Damage;
    public int Reward;
    public int Speed;
    public bool DestroyAfterCollision;

    protected void Start()
    {
        _healthComponent.OnDeath += HandleDeath;
        //same for all child classes
        _movementComponent.MoveAlongPath();
    }

    protected void OnDestroy()
    {
        _healthComponent.OnDeath -= HandleDeath;
    }

    public void Init(EnemyPath path)
    {
        // TODO: Modify this so they have appropriate speed
        _movementComponent.Init(path, Speed);
    }

    protected void HandleDeath()
    {
        // TODO: Modify this so they give appropriate reward
        GameObject.FindObjectOfType<Player>().Resources += Reward;
        OnDeath?.Invoke();
        Destroy(gameObject);
    }

    public void ReduceHealth(int value)
    {
        _healthComponent.HealthValue -= value;
        if (_healthComponent.HealthValue == 0)
        {
            HandleDeath();
        }
    }

    public int GetHealth()
    {
        return this._healthComponent.HealthValue;
    }

}
