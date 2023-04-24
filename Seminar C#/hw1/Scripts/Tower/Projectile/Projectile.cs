using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Rigidbody))]
public abstract class Projectile : MonoBehaviour
{
    [SerializeField] protected Rigidbody _rb;
    [SerializeField] protected LayerMask _enemyLayerMask;
    [SerializeField] protected ParticleSystem _onHitParticleSystem;

    protected float _startTime;
    protected bool _positionsSet = false;
    protected GameObject _gameEnemyObject;

    public int Damage;
    public int Speed;
    public float LengthOfLife;
    public bool DestroyAfterHit;


    private void Start()
    {
        _startTime = Time.time;
    }


    protected virtual void Update()
    {
        if (_positionsSet && (_gameEnemyObject != null))
        {
            Vector3 currentVictimPosition = GetCurrentVictimPosition();
            Vector3 shootDirection = (currentVictimPosition - transform.position).normalized;
            transform.position += shootDirection * Speed * Time.deltaTime;
        }
        if (((Time.time - _startTime) >= LengthOfLife) || _gameEnemyObject == null)
        {
            Destroy(gameObject);
        }
    }

    protected Vector3 GetCurrentVictimPosition()
    {
        if (_gameEnemyObject.GetComponent<LazyEnemy>() != null)
        {
            Vector3 currentPosition = _gameEnemyObject.GetComponent<LazyEnemy>().transform.position;
            // do not hit the floor
            currentPosition.y += 1.0f;
            return currentPosition;
        }
        else
        {
            Vector3 currentPosition = _gameEnemyObject.GetComponent<AggresiveEnemy>().transform.position;
            // do not hit the floor
            currentPosition.y += 1.0f;
            return currentPosition;
        }
    }

    public void StoreVictim(GameObject gameObj)
    {
        _gameEnemyObject = gameObj;
        _positionsSet = true;
    }

    protected virtual void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.GetComponent<LazyEnemy>() != null)
        {
            other.gameObject.GetComponent<LazyEnemy>().ReduceHealth(Damage);
            Destroy(gameObject);
        }
        else if (other.gameObject.GetComponent<AggresiveEnemy>() != null)
        {
            other.gameObject.GetComponent<AggresiveEnemy>().ReduceHealth(Damage);
            Destroy(gameObject);
        }
    }
}
