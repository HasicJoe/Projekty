from flask import Blueprint, render_template, request, flash, redirect, url_for
import json
from flask.typing import ResponseReturnValue

from sqlalchemy.sql.expression import false, over
from . import db
from flask.json import jsonify
from .auth import user_privileges
from flask_login import login_required, current_user
import sqlalchemy as sa
import datetime as dt
import time

passenger = Blueprint('passenger', __name__)


@passenger.route('/search_connection')
def search_connection_view():
    from .models import Link
    if request.method == 'POST':
        enter_stop = request.form.get("enter_stop")
        
    return render_template('passenger_views/search_connection.html')


@passenger.route('/get_stops', methods=['GET'])
def get_stops():
    from .models import Stop
    stop_name = request.args['stop_name'] 
    return jsonify(Stop.query.filter(Stop.name.contains(stop_name)).all()[:10:])


@passenger.route('/get_connection_list', methods=['POST'])
def get_connection_list():
    from .models import Stop, Link, Route, Vehicle, Reservation, Current_location

    enter_stop =    Stop.query.get(request.form.get('enter_stop'))
    exit_stop =     Stop.query.get(request.form.get('exit_stop'))
    date =          request.form.get('date')
    time =          request.form.get('time')

    if not date or not time:
        return {'success' : False, 'msg' : 'Please enter "date" and "time".', 'type' : 'danger' }

    if not enter_stop:
        return {'success' : False, 'msg' : 'Enter stop is invalid. Please enter a valid stop name.', 'type' : 'danger' }

    if not exit_stop:
        return {'success' : False, 'msg' : 'Exit stop is invalid. Please enter a valid stop name.', 'type' : 'danger'}

    # get matching connections
    Connections = Link.query.filter(Link.routes.any(name=enter_stop.name)).filter(Link.routes.any(name=exit_stop.name)).all()

    # get routes for all matched connections
    routes_in_connections = [ Route.query.filter_by(link_id=conn.id).order_by(sa.asc(Route.arrival_time)).all() for conn in Connections ]
    
    data_concat = []
    for routes in routes_in_connections:
        connection = Link.query.get(routes[0].link_id)

        # filter days when connection does not run
        if date and not (connection.interval >> dt.datetime.strptime(date, '%Y-%m-%d').weekday()) & 1:
            continue

        # check if found connection has its enter and stop route in time ascending order
        # match enter and exit stops with their corresponding routes
        wrong_order = -1
        for j, route in enumerate(routes):
            if route.stop_id == enter_stop.name:
                if wrong_order == -1:
                    wrong_order = False 
                enter_route = { 'index' : j, 'name' : route.stop_id }
                enter_route_arrival = route.arrival_time
                        
            if route.stop_id == exit_stop.name:
                if wrong_order == -1:
                    wrong_order = True
                exit_route = { 'index' : j, 'name' : route.stop_id }
        
        vehicle = connection.vehicle

        enter_route_x = Route.query.filter_by(link_id=connection.id, stop_id=enter_stop.name).first()
        input_date = dt.datetime.strptime(date, "%Y-%m-%d")
        input_date_time = dt.datetime.combine(input_date, enter_route_x.departure_time)
        # retrieve all taken seats for this connection
        reservations = Reservation.query.filter_by(connection_id=connection.id, cancelled=False).\
                filter(Reservation.date_time == input_date_time).all()

        overlapping = []

        curr_loc = Current_location.query.filter_by(connection_id=connection.id).first()
        routes_for_curr_loc = Route.query.filter_by(link_id=connection.id).all()
        if curr_loc:
            curr_loc_html = f"<hr></hr><h5 class='ms-5'>Connection is currently located between stops <i>{routes_for_curr_loc[curr_loc.location].stop_id}</i> and <i>{routes_for_curr_loc[curr_loc.location+1].stop_id}</i></h5>"
        else:
            curr_loc_html = ""

        for reservation in reservations:
            routes = Route.query.filter_by(link_id=reservation.connection.id).order_by(sa.asc(Route.arrival_time)).all()
            stops = [ route.stop_id for route in routes ]
            if stops.index(exit_stop.name) >= stops.index(reservation.enter_stop) and stops.index(reservation.exit_stop) >= stops.index(enter_stop.name):
                overlapping.append(reservation)

        f_cls_avail = vehicle.first_class_capacity
        s_cls_avail = vehicle.second_class_capacity

        for reservation in overlapping:
            for seat in reservation.seats:
                if seat.reservation_class == 0:
                    f_cls_avail -= 1
                else:
                    s_cls_avail -= 1

        # check if time of arrival of the enter stop is greater than the
        if time and dt.datetime.strptime(time, "%H:%M").time() > enter_route_arrival:
            continue 

        if not wrong_order:
            data_concat.append({
            'connection' : connection,
            'vehicle' : Vehicle.query.get(connection.vehicle_id),
            'enter_stop' : enter_route,
            'exit_stop' : exit_route,
            'date' : date,
            'time' : time,
            'first_class_available' : f_cls_avail,
            'second_class_available' : s_cls_avail,
            'curr_loc_html' : curr_loc_html,
            'routes' : [{ 'arrival' : route.arrival_time.isoformat('minutes'),
                          'departure' : route.departure_time.isoformat('minutes'),
                          'stop' : route.stop_id } for route in routes ]
                          })

    # return JSON data
    if data_concat:
       return {'success' : True, 'data' : data_concat}
    
    # return when no connections were found
    return {'success' : False, 'msg' : 'No connections fit your search criteria', 'type' : 'danger' }


@passenger.route('/reservation/<connection_id>', methods=['GET', 'POST'])
def reservation_view(connection_id):
    from .models import Reservation, Link, Stop, Route
    connection = Link.query.get(connection_id)
    
    if request.method == 'GET':

        enter_stop =    request.args.get('enter_stop')
        exit_stop =     request.args.get('exit_stop')
        date =          request.args.get('date')
        
        if not enter_stop or not exit_stop:
            return "<h1>INTERNAL SERVER ERROR</h1>"

        vehicle = connection.vehicle

        enter_route = Route.query.filter_by(link_id=connection_id, stop_id=enter_stop).first()
        input_date = dt.datetime.strptime(date, "%Y-%m-%d")
        input_date_time = dt.datetime.combine(input_date, enter_route.departure_time)
     
        # retrieve all taken seats for this connection
        reservations = Reservation.query.filter_by(connection_id=connection_id, cancelled=False).\
                filter(Reservation.date_time == input_date_time).all()

        overlapping = []

        for reservation in reservations:
            routes = Route.query.filter_by(link_id=reservation.connection.id).order_by(sa.asc(Route.arrival_time)).all()
            stops = [ route.stop_id for route in routes ]
            if stops.index(exit_stop) >= stops.index(reservation.enter_stop) and stops.index(reservation.exit_stop) >= stops.index(enter_stop):
                overlapping.append(reservation)

        first_class_taken = []
        second_class_taken = []

        for reservation in overlapping:
            for seat in reservation.seats:
                if seat.reservation_class == 0:
                    first_class_taken.append(str(seat.position))
                else:
                    second_class_taken.append(str(seat.position))
    
        # calculate how many there is in connection for each class
        import math
        type_cap = 60 if vehicle.vehicle_type == 'bus' else 40
        f_cls_carriages = math.ceil(vehicle.first_class_capacity / type_cap)
        s_cls_carriages = math.ceil(vehicle.second_class_capacity / type_cap)

        if current_user.is_authenticated:
            email = current_user.email        
        else:
            email = None

    if request.method == 'POST':
        return '<h1> success </h1>'

    return render_template(
        'passenger_views/create_reservation.html', 
        enter_stop=enter_stop, 
        exit_stop=exit_stop, 
        email=email,
        connection=connection_id,
        vehicle_type=vehicle.vehicle_type,
        first_cls_cap=vehicle.first_class_capacity,
        second_cls_cap=vehicle.second_class_capacity,
        f_cls_car=f_cls_carriages,
        s_cls_car=s_cls_carriages,
        date=date,
        f_cls_taken = ','.join(first_class_taken),
        s_cls_taken = ','.join(second_class_taken)
        )

@passenger.route('/check_email')
def check_email():
    from .models import User
    users = User.query.filter_by(email=request.args.get('email')).all()
    if users:
        return {'success' : False, 'msg' : 'Account with specified email already exists, if this is your email please login', 'type' : 'danger'}
    
    return {'success' : True}

@passenger.route('/create_reservation/<connection_id>', methods=['POST'])
def create_reservation(connection_id):
    if request.method == 'POST':
        from .models import Reservation, Seat, Link, Route

        form = request.form

        connection = Link.query.get(connection_id)
        if not connection:
            return {'success' : False, 'msg' : 'Internal server error', 'type' : 'danger'}
        
        if current_user.is_authenticated:
            email = None
            user = current_user
        else:
            email = form.get('email')
            if not email:
                return {'success' : False, 'msg' : 'Please enter an email', 'type' : 'danger'}
            user = None

        tarif_list = form.getlist('tarif')

        if not tarif_list:
            return {'success' : False, 'msg' : 'Please choose at least one seat', 'type' : 'danger'}

        if str(-1) in tarif_list:
            return {'success' : False, 'msg' : 'Select tarif on all chosen seats', 'type' : 'danger'}

        confirmation_check = form.get('confirmation_check')
        gdpr_confirmation = form.get('gdpr_confirmation')
        if confirmation_check is None or gdpr_confirmation is None:
            return {'success' : False, 'msg' : 'To create a reservation you must accept our terms and conditions', 'type' : 'danger'} 

        # calculate datetime for reservation from entered date in 'connection search' and departure time of enter stop
        res_date = form.get('date-value')
        enter_stop = Route.query.filter_by(stop_id=form.get('enter_stop'),link_id=connection_id).first()

        res_time = enter_stop.departure_time
        date = dt.datetime.strptime(res_date, '%Y-%m-%d')
        date_time = dt.datetime.combine(date, res_time)

        reservation = Reservation(
            confirmed = False,
            cancelled = False,
            enter_stop = form.get('enter_stop'),
            exit_stop = form.get('exit_stop'),
            connection = connection,
            date_time = date_time,
            email = email, 
            user = user
        )

        db.session.add(reservation)
        db.session.commit()

        for _class, _seat, _tarif in zip(form.getlist('class'), form.getlist('seat'), tarif_list):
            seat = Seat(
                reservation_class = _class,
                position = _seat,
                tarif = _tarif
            )
            db.session.add(seat)
            reservation.seats.append(seat)
      
        db.session.commit()    

    # redirect, when no logged in ->
    return {'success' : True, 'email' : email}


@passenger.route('/get_vehicle_info/<vehicle_id>')
@user_privileges(user_role='passenger')
def get_vehicle_info(vehicle_id):
    from .models import Vehicle
    vehicle = Vehicle.query.get(vehicle_id)
    return {
        "type" : vehicle.vehicle_type,
        "first_cap" : vehicle.first_class_capacity,
        "secondary_cap" : vehicle.second_class_capacity,
    }


@passenger.route('/check_availability')
@user_privileges(user_role="passenger") 
def check_availability_view():
    from .models import Link
    if request.method == "POST":
        check_link = request.form.get("check_link")
        
    return render_template('passenger_views/check_availability.html')


@passenger.route('/generate_reservation')
def generate_reservation():
    from .models import Link, User, Reservation, Seat, Vehicle
    from random import choice, randint, randrange

    user = choice(User.query.all())
    connection = choice(Link.query.all())
    
    vehicle = connection.vehicle

    reservation = Reservation(
        confirmed = False,
        cancelled = False,
    )

    reservation.user_id = user.id
    reservation.connection_id = connection.id
    reservation.enter_stop = connection.routes[0].name
    reservation.exit_stop = connection.routes[-1].name


    db.session.add(reservation)
    db.session.commit()

    for i in range(randrange(10)):
        cls = randint(0, 1)
        if cls == 0:
            position = randrange(vehicle.first_class_capacity)
        elif cls == 1:
            position = randrange(vehicle.second_class_capacity)
        seat = Seat(
            position = position,
            tarif = randint(0, 3),
            reservation_class = cls
        )   
        db.session.add(seat)
        db.session.commit()
        reservation.seats.append(seat)

    
    db.session.commit()

    return "<h1>reservation successfully created</h1>"
