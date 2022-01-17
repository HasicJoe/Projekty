from flask import Blueprint
from .models import Role, User, UserRoles, Stop, Vehicle, Link, Route
from random import randrange, sample, randint
import datetime 
import csv
from . import db
from werkzeug.security import generate_password_hash

generator = Blueprint("generator", __name__)


@generator.route("/gen_data", methods=['GET'])
def init_DB_data():
    #generate roles
    available_roles = ["administrator", "operator", "personnel", "passenger"]
    for role in available_roles:
        create_role = Role(name=role)
        db.session.add(create_role)
    db.session.commit()
    # generate basic users
    for role in available_roles:
        new_user = User(login_name=role, password=generate_password_hash(role, method="sha256"), user_name=str("Test " + role), user_surname=str("Surname " +role), email=str(role+"@"+role+".com"), phone_number=str("0915") + str(randrange(100000,999999)))
        r = Role.query.filter_by(name=role).first()
        new_user.roles.append(r)
        db.session.add(new_user)
    db.session.commit()
    #generate operators
    login_names = ['ceske drahy', 'regiojet', 'flix']
    emails = ['cd@cd.cz', 'regio@jet.com', 'flix@bus.net']
    password = '123456'
    for i in range(3):
        operator = User(
            login_name = login_names[i],
            email = emails[i],
            password = generate_password_hash(password, method="sha256"),
            user_name = 'operator ' + str(i),
            user_surname = '',
            phone_number = '',
        )
        db.session.add(operator)
        operator_role = Role.query.filter_by(name="operator").first()
        operator.roles.append(operator_role)
        db.session.commit()
    #generate stops
    counter = 0
    with open('dataset/stops.txt','r') as data:
        data.readline()
        for line in csv.reader(data):
                if counter > 50:
                    break
                if Stop.query.filter_by(name=line[1]).first():
                    continue
                addr_id = randint(0, 169)
                stop = Stop(name=line[1], address=f'{line[1]} {addr_id}', latitude=float(line[2]), longitude=float(line[3]), description=f'This is description for {line[1]} stop', status=1)
                db.session.add(stop)
                db.session.commit()
                counter += 1
    #generate vehicles
    veh_type = ["train", "bus"]
    for i in range(25):
        license_plate = str(chr(randrange(65, 91)))+str(chr(randrange(65, 91)))+'-'+''.join([ str(randrange(10)) for i in range(4) ])
        first_class_cap = randrange(20, 100)
        second_class_cap = randrange(40,200)
        description = 'Vehicle with license plate: ' + license_plate + ' has capacity of ' + str(int(first_class_cap+second_class_cap)) + ' seats.'
        vehicle = Vehicle(
            license_plate = license_plate,
            first_class_capacity = first_class_cap,
            second_class_capacity = second_class_cap,
            description = description,
            vehicle_type = sample(veh_type,1)[0]
        )
        db.session.add(vehicle)
        db.session.commit()
    # generate connection data
    for _i in range(20):
        connection_name = 'connection ' + str(chr(randrange(65, 91))) + str(chr(randrange(65, 91))) + ''.join([ str(randrange(10)) for i in range(6) ])
        operators = User.query.join(User.roles).filter_by(name='operator').all()
        operator = operators[randrange(len(operators))]
        interval = randrange(1, 128)
        vehicles = Vehicle.query.all()
        vehicle = vehicles[randrange(len(vehicles))]

        connection = Link(
            confirmed = True,
            operator = operator.id,
            link_name = connection_name,
            interval = interval,
            vehicle = vehicle
        )

        db.session.add(connection)
        db.session.commit()
        route_count = randrange(3, 21)
        stops = Stop.query.all()
        stop_indices = sample(range(len(stops)), route_count)
        delta = randrange(5, 20)
        start = randrange(5*60, 14*60)
        for i in range(route_count):
            arrival = (start+delta)*60+82800
            arrival_time = datetime.datetime.fromtimestamp(arrival).time()
            departure_time = datetime.datetime.fromtimestamp(arrival+60*randrange(2,4)).time()
            route = Route(
                stop_id = stops[stop_indices[i]].name,
                link_id = connection.id,
                arrival_time = arrival_time,
                departure_time = departure_time
            )
            delta += randrange(7, 20)
            db.session.add(route)
        db.session.commit()
    return "DONE"